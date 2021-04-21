//
// Created by James.Balajan on 31/03/2021.
//

#include <algorithm>
#include <fmt/core.h>
#include <fstream>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "graph.hpp"
#include "visgraph/vistree_generator.hpp"

#define BITS_IN_A_BYTE 8u

void serialize_to_stream(std::ostream& stream, double val);
void serialize_to_stream(std::ostream& stream, uint64_t val);
void serialize_to_stream(std::ostream& stream, uint8_t val);

bool deserialize_from_stream(std::istream& stream, double& val);
bool deserialize_from_stream(std::istream& stream, uint64_t& val);
bool deserialize_from_stream(std::istream& stream, uint8_t& val);

Graph::Graph(std::vector<Polygon> polygons) : _polygons(std::move(polygons)) {}

Graph::Graph(const Graph &other_graph) : _neighbors(other_graph._neighbors), _polygons(other_graph._polygons) {}

void Graph::add_edge(const Coordinate &a, const Coordinate &b) {
    add_directed_edge(a, b);
    add_directed_edge(b, a);
}

void Graph::add_directed_edge(const Coordinate &a, const Coordinate &b) {
    decltype(_neighbors)::accessor accessor;

    _neighbors.insert(accessor, a);
    accessor->second.insert(b);
    accessor.release();
}

bool Graph::has_edge(const Coordinate &a, const Coordinate &b) const {
    decltype(_neighbors)::const_accessor accessor;

    const auto found_a_in_neighbors = _neighbors.find(accessor, a);

    const auto has_edge_result = (found_a_in_neighbors) && (accessor->second.find(b) != accessor->second.end());

    accessor.release();

    return has_edge_result;
}

std::string Graph::to_string_representation() const {
    auto outs = std::stringstream();

    outs << "Graph (\n";
    for (const auto &neighbors : _neighbors) {
        outs << fmt::format("\t[({}, {}) [", neighbors.first.get_longitude(), neighbors.first.get_latitude());
        for (const auto &neighbor : neighbors.second) {
            outs << fmt::format("({}, {}) ", neighbor.get_longitude(), neighbor.get_latitude());
        }
        outs << "]]\n";
    }
    outs << ")";

    return outs.str();
}

bool Graph::operator==(const Graph &other) const {
    for (const auto &_neighbor : _neighbors) {
        decltype(other._neighbors)::const_accessor accessor;
        const auto found_matching_neighbor = other._neighbors.find(accessor, _neighbor.first);

        if (!found_matching_neighbor || _neighbor.second != accessor->second) {
            return false;
        }

        accessor.release();
    }

    return _polygons == other._polygons;
}

bool Graph::operator!=(const Graph &other) const { return !(*this == other); }

void Graph::serialize_to_file(const std::string &path) const {
    const uint64_t num_polygons = _polygons.size();
    const auto vertices = get_vertices();
    const uint64_t num_vertices = vertices.size();

    auto adjacency_matrix = std::vector<std::vector<bool>>(num_vertices, std::vector<bool>(num_vertices, false));
    for (size_t i = 0; i < num_vertices; ++i) {
        for (size_t j = 0; j < num_vertices; ++j) {
            const auto vertex_i = vertices[i];
            const auto vertex_j = vertices[j];

            decltype(_neighbors)::const_accessor accessor;
            const auto found = _neighbors.find(accessor, vertex_i);
            if (found && accessor->second.find(vertex_j) != accessor->second.end()) {
                adjacency_matrix[i][j] = true;
            }

            accessor.release();
        }
    }

    auto file_stream = std::ofstream(path);
    if (file_stream.is_open()) {
        serialize_to_stream(file_stream, num_polygons);
        for (const auto &polygon : _polygons) {
            const uint64_t num_vertices_in_polygon = polygon.get_vertices().size();
            serialize_to_stream(file_stream, num_vertices_in_polygon);
            for (const auto &vertex : polygon.get_vertices()) {
                const double longitude = vertex.get_longitude();
                const double latitude = vertex.get_latitude();

                serialize_to_stream(file_stream, longitude);
                serialize_to_stream(file_stream, latitude);
            }
        }

        for (size_t i = 0; i < num_vertices; ++i) {
            for (size_t j = 0; j < i / BITS_IN_A_BYTE + (i % BITS_IN_A_BYTE != 0); ++j) {
                uint8_t adjacency_encoding = 0x0;

                for (size_t l = j * BITS_IN_A_BYTE; l < std::min((j + 1) * BITS_IN_A_BYTE, i + 1); ++l) {
                    adjacency_encoding |= (static_cast<uint8_t>(adjacency_matrix[i][l]) << (l - j * BITS_IN_A_BYTE));
                }

                serialize_to_stream(file_stream, adjacency_encoding);
            }
        }

        file_stream.close();
    } else {
        throw std::runtime_error(fmt::format("Could not open file {}", path));
    }
}

Graph Graph::load_from_file(const std::string &path) {
    auto file_stream = std::ifstream(path);
    auto file_read_str = std::string();
    if (file_stream.is_open()) {
        uint64_t num_polygons;
        if (!deserialize_from_stream(file_stream, num_polygons)) {
            throw std::runtime_error("Num polygon header not present");
        }

        auto all_vertices = std::vector<Coordinate>();
        auto polygons = std::vector<Polygon>();
        polygons.reserve(num_polygons);
        for (size_t i = 0; i < num_polygons; ++i) {
            uint64_t num_vertices;
            if (!deserialize_from_stream(file_stream, num_vertices)) {
                throw std::runtime_error(fmt::format("{}th polygon missing num vertices", i));
            }

            auto vertices = std::vector<Coordinate>();
            vertices.reserve(num_vertices);
            for (size_t j = 0; j < num_vertices; ++j) {
                double longitude, latitude;
                if (!deserialize_from_stream(file_stream, longitude) ||
                    !deserialize_from_stream(file_stream, latitude)) {
                    throw std::runtime_error(fmt::format("{}th vertex of {}th polygon not found", j, i));
                }

                const auto coord = Coordinate(longitude, latitude);
                vertices.push_back(coord);
                all_vertices.push_back(coord);
            }

            polygons.emplace_back(vertices);
        }

        auto graph = Graph(polygons);
        const auto num_vertices = all_vertices.size();

        for (size_t i = 0; i < num_vertices; ++i) {
            for (size_t j = 0; j < i / BITS_IN_A_BYTE + (i % BITS_IN_A_BYTE != 0); ++j) {
                uint8_t encoded_adjacency;
                if (!deserialize_from_stream(file_stream, encoded_adjacency)) {
                    throw std::runtime_error(
                        fmt::format("{}th row {}th column adjacency matrix entry not found", i, j * 8));
                }

                for (int l = 0; l < std::min(static_cast<size_t>(BITS_IN_A_BYTE), num_vertices - (j * BITS_IN_A_BYTE));
                     ++l) {
                    if ((encoded_adjacency >> l) & 0x1) {
                        graph.add_edge(all_vertices[i], all_vertices[j * BITS_IN_A_BYTE + l]);
                    }
                }
            }
        }

        file_stream.close();

        return graph;
    } else {
        throw std::runtime_error(fmt::format("Could not open file {}", path));
    }
}

struct AStarHeapElement {
    Coordinate node;
    double distance_to_source;
    double heuristic_distance_to_destination;
};

std::vector<Coordinate> Graph::shortest_path(const Coordinate &source, const Coordinate &destination) const {
    auto modified_graph = Graph(*this);

    decltype(modified_graph._neighbors)::const_accessor accessor;
    const auto found_source = modified_graph._neighbors.find(accessor, source);
    if (!found_source) {
        for (const auto &visible_vertex :
             VistreeGenerator::get_visible_vertices_from_root(source, modified_graph._polygons, false)) {
            modified_graph.add_edge(source, visible_vertex);
        }
    }
    accessor.release();

    const auto found_destination = modified_graph._neighbors.find(accessor, destination);
    if (!found_destination) {
        for (const auto &visible_vertex :
             VistreeGenerator::get_visible_vertices_from_root(destination, modified_graph._polygons, false)) {
            modified_graph.add_edge(destination, visible_vertex);
        }
    }
    accessor.release();

    const auto distance_measurement = [](const Coordinate &a, const Coordinate &b) { return (a - b).magnitude(); };

    const auto comparison_func = [&](const AStarHeapElement &a, const AStarHeapElement &b) {
        return (a.distance_to_source + a.heuristic_distance_to_destination) >
               (b.distance_to_source + b.heuristic_distance_to_destination);
    };
    auto pq = std::priority_queue<AStarHeapElement, std::vector<AStarHeapElement>, decltype(comparison_func)>(
        comparison_func);
    pq.push(AStarHeapElement{
        .node = source,
        .distance_to_source = 0,
        .heuristic_distance_to_destination = distance_measurement(source, destination),
    });

    auto prev_coord = std::unordered_map<Coordinate, Coordinate>();
    auto distances_to_source = std::unordered_map<Coordinate, double>();
    while (!pq.empty()) {
        const auto top = pq.top();
        pq.pop();

        if (top.node == destination) {
            break;
        }

        decltype(modified_graph._neighbors)::const_accessor neighbor_accessor;
        modified_graph._neighbors.find(neighbor_accessor, top.node);
        for (const auto &neighbor : neighbor_accessor->second) {
            const auto neighbor_dist_to_source = top.distance_to_source + distance_measurement(neighbor, top.node);

            if (distances_to_source.find(neighbor) != distances_to_source.end() &&
                distances_to_source.at(neighbor) <= neighbor_dist_to_source) {
                continue;
            }

            const auto heap_elem = AStarHeapElement{
                .node = neighbor,
                .distance_to_source = neighbor_dist_to_source,
                .heuristic_distance_to_destination = distance_measurement(neighbor, destination),
            };
            pq.push(heap_elem);
            prev_coord[neighbor] = top.node;
            distances_to_source[neighbor] = neighbor_dist_to_source;
        }
        neighbor_accessor.release();
    }

    auto path = std::vector<Coordinate>();
    auto curr_node = destination;

    while (curr_node != source) {
        path.push_back(curr_node);

        if (prev_coord.find(curr_node) == prev_coord.end()) {
            throw std::runtime_error("Could not find a shortest path");
        }
        curr_node = prev_coord.at(curr_node);
    }
    path.push_back(source);

    std::reverse(path.begin(), path.end());

    return path;
}
std::vector<Coordinate> Graph::get_neighbors(const Coordinate &vertex) const {
    decltype(_neighbors)::const_accessor accessor;
    const auto found_neighbors = _neighbors.find(accessor, vertex);

    auto neighbors = std::vector<Coordinate>();
    if (found_neighbors) {
        neighbors = std::vector<Coordinate>(accessor->second.begin(), accessor->second.end());
    }

    accessor.release();

    return neighbors;
}

std::vector<Coordinate> Graph::get_vertices() const {
    auto vertices = std::vector<Coordinate>();

    for (const auto &polygon : _polygons) {
        for (const auto &vertex : polygon.get_vertices()) {
            vertices.push_back(vertex);
        }
    }

    return vertices;
}

std::vector<Polygon> Graph::get_polygons() const { return _polygons; }

std::ostream &operator<<(std::ostream &outs, const Graph &graph) { return outs << graph.to_string_representation(); }

void serialize_to_stream(std::ostream& stream, double val) {
    uint64_t val_bytes = 0x0;
    std::memcpy(&val_bytes, &val, sizeof(val_bytes));
    serialize_to_stream(stream, val_bytes);
}

void serialize_to_stream(std::ostream& stream, uint64_t val) {
    for (int byte_num = 0; byte_num < 8; ++byte_num) {
        uint8_t byte = (val >> (byte_num * BITS_IN_A_BYTE)) & 0xFFu;
        stream.write((char*) &byte, 1);
    }
}

void serialize_to_stream(std::ostream& stream, uint8_t val) {
    stream.write((char*) &val, 1);
}

bool deserialize_from_stream(std::istream& stream, double& val) {
    uint64_t val_bytes;
    if (!deserialize_from_stream(stream, val_bytes)) {
        return false;
    }
    std::memcpy(&val, &val_bytes, sizeof(val));

    return true;
}

bool deserialize_from_stream(std::istream& stream, uint64_t& val) {
    val = 0x0;
    for (int byte_num = 0; byte_num < 8; ++byte_num) {
        uint8_t curr_byte;
        if (!deserialize_from_stream(stream, curr_byte)) {
            return false;
        }
        val |= (static_cast<uint64_t>(curr_byte) << (byte_num * BITS_IN_A_BYTE));
    }

    return true;
}

bool deserialize_from_stream(std::istream& stream, uint8_t& val) {
    return static_cast<bool>(stream.read((char *) &val, 1));
}

#undef BITS_IN_A_BYTE
