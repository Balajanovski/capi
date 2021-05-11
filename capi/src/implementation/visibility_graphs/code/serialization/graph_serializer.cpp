//
// Created by James.Balajan on 11/05/2021.
//

#include <cstdint>
#include <vector>
#include <mio.hpp>
#include <string>
#include <unistd.h>
#include <fmt/core.h>
#include <system_error>

#include "graph_serializer.hpp"

#define BITS_IN_A_BYTE 8u
#define CEIL_DIV(x, y) (((x) / (y)) + ((x) % (y) != 0))


void serialize_to_mmap(mio::mmap_sink& mmap, uint64_t val, size_t offset);
void serialize_to_mmap(mio::mmap_sink& mmap, double val, size_t offset);
void serialize_to_mmap(mio::mmap_sink& mmap, uint8_t val, size_t offset);

void allocate_file(const std::string& path, size_t num_bytes);
void handle_mmap_error(const std::error_code& error);

void GraphSerializer::serialize_to_file(const Graph& graph, const std::string &path) {
    const auto num_bytes_for_graph = GraphSerializer::calculate_number_of_bytes_for_graph(graph);
    allocate_file(path, num_bytes_for_graph);

    std::error_code error;
    auto rw_mmap = mio::make_mmap_sink(path, 0, mio::map_entire_file, error);
    if (error) { handle_mmap_error(error); }

    const auto& polygons = graph.get_polygons();
    const uint64_t num_polygons = graph.get_polygons().size();

    serialize_to_mmap(rw_mmap, num_polygons, 0);
    const auto poly_offset = serialize_polygon_vertices_to_mmap(rw_mmap, graph, sizeof(num_polygons));
    serialize_adjacency_matrix_to_mmap(rw_mmap, graph, poly_offset);

    allocate_file(path, num_bytes_for_graph);
}

Graph GraphSerializer::deserialize_from_file(const std::string &path) {

}

size_t GraphSerializer::serialize_polygon_vertices_to_mmap(mio::mmap_sink& mmap, const Graph &graph, size_t offset) {
    const auto num_polygons = graph.get_polygons().size();
    const auto& polygons = graph.get_polygons();

    auto polygon_byte_offsets = std::vector<size_t>(num_polygons+1);
    polygon_byte_offsets[0] = offset;
    for (size_t i = 1; i <= num_polygons; ++i) {
        polygon_byte_offsets[i] = polygon_byte_offsets[i-1] +
                                  sizeof(uint64_t) +
                                  (polygons[i-1].get_vertices().size() * sizeof(double) * 2);
    }

#pragma omp parallel for shared(num_polygons, polygons, mmap, polygon_byte_offsets) default(none)
    for (size_t i = 0; i < num_polygons; ++i) {
        const auto& polygon_vertices = polygons[i].get_vertices();

        serialize_to_mmap(mmap, polygon_vertices.size(), polygon_byte_offsets[i]);
        for (size_t j = 0; j < polygon_vertices.size(); ++j) {
            serialize_to_mmap(mmap, polygon_vertices[j].get_longitude(),
                              polygon_byte_offsets[i] + (sizeof(double) * (2 * j)));
            serialize_to_mmap(mmap, polygon_vertices[j].get_latitude(),
                              polygon_byte_offsets[i] + (sizeof(double) * ((2 * j) + 1)));
        }
    }

    return polygon_byte_offsets.back();
}

size_t GraphSerializer::serialize_adjacency_matrix_to_mmap(mio::mmap_sink& mmap, const Graph &graph,
                                                                 size_t offset) {
    const auto& vertices = graph.get_vertices();
    const uint64_t num_vertices = graph.get_vertices().size();

    auto adjacency_matrix_byte_offsets = std::vector<size_t>(num_vertices+1);
    adjacency_matrix_byte_offsets[0] = offset;
    for (size_t i = 1; i <= num_vertices; ++i) {
        adjacency_matrix_byte_offsets[i] = adjacency_matrix_byte_offsets[i-1] + CEIL_DIV(i-1, BITS_IN_A_BYTE);
    }

#pragma omp parallel for shared(num_vertices, mmap, vertices, graph, adjacency_matrix_byte_offsets) default(none)
    for (size_t i = 0; i < num_vertices; ++i) {
        for (size_t j = 0; j < CEIL_DIV(i, BITS_IN_A_BYTE); ++j) {
            uint8_t adjacency_encoding = 0x0;

            for (size_t l = j * BITS_IN_A_BYTE; l < std::min((j + 1) * BITS_IN_A_BYTE, i+1); ++l) {
                const auto vertex_1 = vertices[i];
                const auto vertex_2 = vertices[l];

                adjacency_encoding |= (static_cast<uint8_t>(graph.are_adjacent(vertex_1, vertex_2)) << (l - j * BITS_IN_A_BYTE));
            }

            serialize_to_mmap(mmap, adjacency_encoding,
                              adjacency_matrix_byte_offsets[i] + (j * sizeof(uint8_t)));
        }
    }

    return adjacency_matrix_byte_offsets.back();
}

size_t GraphSerializer::calculate_number_of_bytes_for_graph(const Graph& graph) {
    const auto num_polygons = graph.get_polygons().size();
    const auto num_vertices = graph.get_vertices().size();

    const auto num_verts_over_bits_in_byte_floor = num_vertices / BITS_IN_A_BYTE;
    const auto num_verts_over_bits_in_byte_ceil = CEIL_DIV(num_vertices, BITS_IN_A_BYTE);
    const auto num_adjacency_matrix_bytes = BITS_IN_A_BYTE *
                                            (((num_verts_over_bits_in_byte_floor * num_verts_over_bits_in_byte_floor) +
                           num_verts_over_bits_in_byte_floor) / 2) +
        (num_vertices - BITS_IN_A_BYTE * num_verts_over_bits_in_byte_floor) * num_verts_over_bits_in_byte_ceil;

    return (
        sizeof(uint64_t) + // For the number of polygons header
        sizeof(uint64_t) * num_polygons + // For the number of vertices per polygon headers
        sizeof(uint64_t) * 2 * num_vertices + // For the vertex latitudes and longitudes
        sizeof(uint8_t) * num_adjacency_matrix_bytes
    );
}

inline void serialize_to_mmap(mio::mmap_sink& mmap, uint64_t val, size_t offset) {
    for (size_t byte_num = 0; byte_num < sizeof(uint64_t); ++byte_num) {
        uint8_t byte = (val >> (byte_num * BITS_IN_A_BYTE)) & 0xFFu;
        serialize_to_mmap(mmap, byte, offset + byte_num);
    }
}

inline void serialize_to_mmap(mio::mmap_sink& mmap, double val, size_t offset) {
    uint64_t val_bytes = 0x0;
    std::memcpy(&val_bytes, &val, sizeof(val_bytes));
    serialize_to_mmap(mmap, val_bytes, offset);
}

inline void serialize_to_mmap(mio::mmap_sink& mmap, uint8_t val, size_t offset) {
    mmap[offset] = val;
}

inline void allocate_file(const std::string& path, size_t num_bytes) {
    if (truncate(path.c_str(), num_bytes) != 0) {
        constexpr auto buffer_len = 100;
        char error_buffer[buffer_len];
        strerror_r(errno, error_buffer, buffer_len);

        throw std::runtime_error(fmt::format("Error when allocating file {}. Msg: {}", path, error_buffer));
    }
}

inline void handle_mmap_error(const std::error_code& error) {
    throw std::runtime_error(fmt::format("Error mapping file (error_code: {}): {}",
                                         error.value(), error.message()));
}

#undef BITS_IN_A_BYTE
#undef CEIL_DIV
