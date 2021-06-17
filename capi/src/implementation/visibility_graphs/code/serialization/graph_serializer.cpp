//
// Created by James.Balajan on 11/05/2021.
//

#include <atomic>
#include <cstdint>
#include <fmt/core.h>
#include <mio.hpp>
#include <string>
#include <system_error>
#include <unistd.h>
#include <vector>

#include "graph_serializer.hpp"

#define BITS_IN_A_BYTE 8u
#define CEIL_DIV(x, y) (((x) / (y)) + ((x) % (y) != 0))

void serialize_to_mmap(mio::mmap_sink &mmap, uint64_t val, size_t offset);
void serialize_to_mmap(mio::mmap_sink &mmap, uint32_t val, size_t offset);
void serialize_to_mmap(mio::mmap_sink &mmap, int32_t val, size_t offset);
void serialize_to_mmap(mio::mmap_sink &mmap, uint8_t val, size_t offset);

uint64_t deserialize_8_bytes_from_mmap(const mio::mmap_source &mmap, size_t offset);
uint32_t deserialize_4_bytes_from_mmap(const mio::mmap_source &mmap, size_t offset);
int32_t deserialize_4_signed_bytes_from_mmap(const mio::mmap_source &mmap, size_t offset);
uint8_t deserialize_byte_from_mmap(const mio::mmap_source &mmap, size_t offset);

void allocate_file(const std::string &path, size_t num_bytes);
void handle_mmap_error(const std::error_code &error);

void GraphSerializer::serialize_to_file(const Graph &graph, const std::string &path) {
    const auto num_meridian_spanning_edges =
        GraphSerializer::calculate_number_of_meridian_spanning_edges_for_graph(graph);
    const auto num_bytes_for_graph =
        GraphSerializer::calculate_number_of_bytes_for_graph(graph, num_meridian_spanning_edges);
    allocate_file(path, num_bytes_for_graph);

    std::error_code error;
    auto rw_mmap = mio::make_mmap_sink(path, 0, mio::map_entire_file, error);
    if (error) {
        handle_mmap_error(error);
    }

    const auto &polygons = graph.get_polygons();
    const uint64_t num_polygons = graph.get_polygons().size();

    serialize_to_mmap(rw_mmap, num_polygons, 0);
    const auto poly_offset = serialize_polygon_vertices_to_mmap(rw_mmap, graph, sizeof(num_polygons));
    serialize_adjacency_matrix_to_mmap(rw_mmap, graph, poly_offset, num_meridian_spanning_edges);

    rw_mmap.sync(error);
    if (error) {
        handle_mmap_error(error);
    }

    rw_mmap.unmap();
}

Graph GraphSerializer::deserialize_from_file(const std::string &path) {
    std::error_code error;
    auto r_mmap = mio::make_mmap_source(path, 0, mio::map_entire_file, error);
    if (error) {
        handle_mmap_error(error);
    }

    const auto num_polygons = deserialize_8_bytes_from_mmap(r_mmap, 0);

    Graph graph;
    const auto poly_offset = deserialize_polygon_vertices_from_mmap(r_mmap, graph, num_polygons, sizeof(num_polygons));
    deserialize_adjacency_matrix_from_mmap(r_mmap, graph, poly_offset);

    r_mmap.unmap();

    return graph;
}

size_t GraphSerializer::serialize_polygon_vertices_to_mmap(mio::mmap_sink &mmap, const Graph &graph, size_t offset) {
    const auto num_polygons = graph.get_polygons().size();
    const auto &polygons = graph.get_polygons();

    auto polygon_byte_offsets = std::vector<size_t>(num_polygons + 1);
    polygon_byte_offsets[0] = offset;
    for (size_t i = 1; i <= num_polygons; ++i) {
        polygon_byte_offsets[i] = polygon_byte_offsets[i - 1] + sizeof(uint64_t) +
                                  (polygons[i - 1].get_vertices().size() * sizeof(int32_t) * 2);
    }

    for (size_t i = 0; i < num_polygons; ++i) {
        const auto &polygon_vertices = polygons[i].get_vertices();
        uint64_t num_polygon_vertices = polygon_vertices.size();

        serialize_to_mmap(mmap, num_polygon_vertices, polygon_byte_offsets[i]);

        for (size_t j = 0; j < num_polygon_vertices; ++j) {
            const auto longitude_offset =
                polygon_byte_offsets[i] + sizeof(num_polygon_vertices) + (sizeof(int32_t) * (2 * j));
            const auto latitude_offset = longitude_offset + sizeof(int32_t);
            serialize_to_mmap(mmap, polygon_vertices[j].get_longitude_microdegrees(), longitude_offset);
            serialize_to_mmap(mmap, polygon_vertices[j].get_latitude_microdegrees(), latitude_offset);
        }
    }

    return polygon_byte_offsets.back();
}

size_t GraphSerializer::serialize_adjacency_matrix_to_mmap(mio::mmap_sink &mmap, const Graph &graph, size_t offset,
                                                           uint64_t num_meridian_spanning_edges) {
    const auto &vertices = graph.get_vertices();
    uint64_t num_vertices = graph.get_vertices().size();

    auto adjacency_matrix_byte_offsets = std::vector<size_t>(num_vertices + 1);
    adjacency_matrix_byte_offsets[0] = offset;
    for (size_t i = 1; i <= num_vertices; ++i) {
        adjacency_matrix_byte_offsets[i] = adjacency_matrix_byte_offsets[i - 1] + CEIL_DIV(i - 1, BITS_IN_A_BYTE);
    }

    for (size_t i = 0; i < num_vertices; ++i) {
        for (size_t j = 0; j < CEIL_DIV(i, BITS_IN_A_BYTE); ++j) {
            uint8_t adjacency_encoding = 0x0;

            for (size_t l = j * BITS_IN_A_BYTE; l < std::min((j + 1) * BITS_IN_A_BYTE, i + 1); ++l) {
                const auto vertex_1 = vertices[i];
                const auto vertex_2 = vertices[l];

                adjacency_encoding |=
                    (static_cast<uint8_t>(graph.are_adjacent(vertex_1, vertex_2)) << (l - j * BITS_IN_A_BYTE));
            }

            serialize_to_mmap(mmap, adjacency_encoding, adjacency_matrix_byte_offsets[i] + (j * sizeof(uint8_t)));
        }
    }

    auto adjacency_matrix_byte_offsets_back = adjacency_matrix_byte_offsets.back();
    std::atomic<uint64_t> current_meridian_spanning_edge = 0;
    serialize_to_mmap(mmap, num_meridian_spanning_edges, adjacency_matrix_byte_offsets_back);
    for (size_t i = 0; i < num_vertices; ++i) {
        for (size_t j = 0; j < i; ++j) {
            const uint64_t edge_number = j + (((i * i) + i) / 2);
            if (graph.is_edge_meridian_crossing(vertices[i], vertices[j])) {
                serialize_to_mmap(mmap, edge_number,
                                  ((current_meridian_spanning_edge++) + 1) * sizeof(uint64_t) +
                                      adjacency_matrix_byte_offsets_back);
            }
        }
    }

    return adjacency_matrix_byte_offsets_back + (num_meridian_spanning_edges + 1) * sizeof(uint64_t);
}

size_t GraphSerializer::deserialize_polygon_vertices_from_mmap(const mio::mmap_source &mmap, Graph &graph,
                                                               uint64_t num_polygons, size_t offset) {
    auto polygons = std::vector<Polygon>(num_polygons);

    size_t curr_offset = offset;
    for (size_t i = 0; i < num_polygons; ++i) {
        auto num_vertices = deserialize_8_bytes_from_mmap(mmap, curr_offset);
        curr_offset += sizeof(num_vertices);

        auto vertices = std::vector<Coordinate>(num_vertices);
        for (size_t j = 0; j < num_vertices; ++j) {
            const auto longitude_offset = curr_offset + (j * 2 * sizeof(int32_t));
            const auto latitude_offset = longitude_offset + sizeof(int32_t);
            const auto longitude = deserialize_4_signed_bytes_from_mmap(mmap, longitude_offset);
            const auto latitude = deserialize_4_signed_bytes_from_mmap(mmap, latitude_offset);
            vertices[j] = Coordinate(longitude, latitude);
        }

        curr_offset += (num_vertices * 2 * sizeof(int32_t));

        polygons[i] = Polygon(vertices);
    }

    graph = Graph(polygons);

    return curr_offset;
}

size_t GraphSerializer::deserialize_adjacency_matrix_from_mmap(const mio::mmap_source &mmap, Graph &graph,
                                                               size_t offset) {
    auto num_vertices = graph.get_vertices().size();
    const auto &vertices = graph.get_vertices();
    auto adjacency_matrix_byte_offsets = std::vector<size_t>(num_vertices + 1);
    adjacency_matrix_byte_offsets[0] = offset;
    for (size_t i = 1; i <= num_vertices; ++i) {
        adjacency_matrix_byte_offsets[i] = adjacency_matrix_byte_offsets[i - 1] + CEIL_DIV(i - 1, BITS_IN_A_BYTE);
    }

    std::unordered_set<uint64_t> meridian_spanning_edge_indices;
    const auto num_meridian_spanning_edges = deserialize_8_bytes_from_mmap(mmap, adjacency_matrix_byte_offsets.back());
    for (size_t i = 0; i < num_meridian_spanning_edges; ++i) {
        meridian_spanning_edge_indices.insert(
            deserialize_8_bytes_from_mmap(mmap, adjacency_matrix_byte_offsets.back() + (i + 1) * sizeof(uint64_t)));
    }

    for (size_t i = 0; i < num_vertices; ++i) {
        for (size_t j = 0; j < CEIL_DIV(i, BITS_IN_A_BYTE); ++j) {
            const auto encoded_adjacency =
                deserialize_byte_from_mmap(mmap, adjacency_matrix_byte_offsets[i] + (j * sizeof(uint8_t)));

            for (size_t l = 0; l < std::min(static_cast<size_t>(BITS_IN_A_BYTE), num_vertices - (j * BITS_IN_A_BYTE));
                 ++l) {
                if ((encoded_adjacency >> l) & 0x1) {
                    const auto neighbor_index = j * BITS_IN_A_BYTE + l;
                    const auto edge_index = neighbor_index + (((i * i) + i) / 2);
                    graph.add_edge(vertices[i], vertices[neighbor_index],
                                   meridian_spanning_edge_indices.find(edge_index) !=
                                       meridian_spanning_edge_indices.end());
                }
            }
        }
    }

    return adjacency_matrix_byte_offsets.back();
}

size_t GraphSerializer::calculate_number_of_bytes_for_graph(const Graph &graph, uint64_t num_meridian_spanning_edges) {
    const auto num_polygons = graph.get_polygons().size();
    const auto num_vertices = graph.get_vertices().size();

    const auto num_verts_over_bits_in_byte_floor = num_vertices / BITS_IN_A_BYTE;
    const auto num_verts_over_bits_in_byte_ceil = CEIL_DIV(num_vertices, BITS_IN_A_BYTE);
    const auto num_adjacency_matrix_bytes =
        BITS_IN_A_BYTE * (((num_verts_over_bits_in_byte_floor * num_verts_over_bits_in_byte_floor) +
                           num_verts_over_bits_in_byte_floor) /
                          2) +
        (num_vertices - BITS_IN_A_BYTE * num_verts_over_bits_in_byte_floor) * num_verts_over_bits_in_byte_ceil;

    return (sizeof(uint64_t) +                             // For the number of polygons header
            sizeof(uint64_t) * num_polygons +              // For the number of vertices per polygon headers
            sizeof(int32_t) * 2 * num_vertices +           // For the vertex latitudes and longitudes
            sizeof(uint8_t) * num_adjacency_matrix_bytes + // For the adjacency matrix
            sizeof(uint64_t) *
                (num_meridian_spanning_edges + 1)); // For the number of meridian spanning edges, and each edge index;
}

uint64_t GraphSerializer::calculate_number_of_meridian_spanning_edges_for_graph(const Graph &graph) {
    size_t meridian_spanning_edges = 0;
    for (const auto &vertex_1 : graph.get_vertices()) {
        for (const auto &vertex_2 : graph.get_vertices()) {
            meridian_spanning_edges += graph.is_edge_meridian_crossing(vertex_1, vertex_2);
        }
    }

    return meridian_spanning_edges;
}

inline void serialize_to_mmap(mio::mmap_sink &mmap, uint64_t val, size_t offset) {
    for (size_t byte_num = 0; byte_num < sizeof(val); ++byte_num) {
        uint8_t byte = (val >> (byte_num * BITS_IN_A_BYTE)) & 0xFFu;
        serialize_to_mmap(mmap, byte, offset + byte_num);
    }
}

inline void serialize_to_mmap(mio::mmap_sink &mmap, uint32_t val, size_t offset) {
    for (size_t byte_num = 0; byte_num < sizeof(val); ++byte_num) {
        uint8_t byte = (val >> (byte_num * BITS_IN_A_BYTE)) & 0xFFu;
        serialize_to_mmap(mmap, byte, offset + byte_num);
    }
}

inline void serialize_to_mmap(mio::mmap_sink &mmap, int32_t val, size_t offset) {
    uint32_t val_bytes = 0x0;
    std::memcpy(&val_bytes, &val, sizeof(val_bytes));
    serialize_to_mmap(mmap, val_bytes, offset);
}

inline void serialize_to_mmap(mio::mmap_sink &mmap, uint8_t val, size_t offset) { mmap[offset] = val; }

inline uint64_t deserialize_8_bytes_from_mmap(const mio::mmap_source &mmap, size_t offset) {
    uint64_t val = 0x0;
    for (int i = 0; i < sizeof(val); ++i) {
        const auto curr_byte = deserialize_byte_from_mmap(mmap, offset + i);
        val |= (static_cast<uint64_t>(curr_byte) << (i * BITS_IN_A_BYTE));
    }
    return val;
}

inline uint32_t deserialize_4_bytes_from_mmap(const mio::mmap_source &mmap, size_t offset) {
    uint32_t val = 0x0;
    for (int i = 0; i < sizeof(val); ++i) {
        const auto curr_byte = deserialize_byte_from_mmap(mmap, offset + i);
        val |= (static_cast<uint32_t>(curr_byte) << (i * BITS_IN_A_BYTE));
    }
    return val;
}

inline int32_t deserialize_4_signed_bytes_from_mmap(const mio::mmap_source &mmap, size_t offset) {
    uint32_t bytes_val = deserialize_4_bytes_from_mmap(mmap, offset);

    int32_t val;
    std::memcpy(&val, &bytes_val, sizeof(val));

    return val;
}

inline uint8_t deserialize_byte_from_mmap(const mio::mmap_source &mmap, size_t offset) { return mmap[offset]; }

inline void allocate_file(const std::string &path, size_t num_bytes) {
    const auto handle_allocation_error = [&]() {
        constexpr auto buffer_len = 100;
        char error_buffer[buffer_len];
        const auto linux_error = strerror_r(errno, error_buffer, buffer_len);

        const auto error = fmt::format("Error when allocating file {}. Errno: {}. Msg: {}", path, errno, linux_error);
        throw std::runtime_error(error);
    };

    const auto fd = open(path.c_str(), O_CREAT | O_RDWR, S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP | S_IRWXU);
    if (fd == -1) {
        handle_allocation_error();
    }
    if (ftruncate(fd, num_bytes) != 0) {
        handle_allocation_error();
    }
    if (close(fd) == -1) {
        handle_allocation_error();
    }
}

inline void handle_mmap_error(const std::error_code &error) {
    const auto error_msg = fmt::format("Error mapping file (error_code: {}): {}", error.value(), error.message());
    throw std::runtime_error(error_msg);
}
