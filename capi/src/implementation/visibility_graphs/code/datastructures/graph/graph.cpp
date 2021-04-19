//
// Created by James.Balajan on 31/03/2021.
//

#include <fmt/core.h>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>

#include "visgraph/vistree_generator.hpp"
#include "graph.hpp"

Graph::Graph(const std::vector<Polygon>& polygons): _polygons(polygons) {
    size_t num_vertices = 0;
    for (const auto& polygon: polygons) {
        num_vertices += polygon.get_vertices().size();
    }

    _neighbors.reserve(num_vertices);
}

Graph::Graph(const Graph& other_graph): _neighbors(other_graph._neighbors), _polygons(other_graph._polygons) {}

void Graph::add_edge(const Coordinate &a, const Coordinate &b) {
    add_directed_edge(a, b);
    add_directed_edge(b, a);
}

void Graph::add_directed_edge(const Coordinate &a, const Coordinate &b) {
    const auto mutex_index = std::hash<Coordinate>()(a) % NUM_COORDINATE_MUTEXES;
    _coordinate_bucket_access_mutexes[mutex_index].lock();

    if (_neighbors.find(a) == _neighbors.end()) {
        _neighbors[a] = std::unordered_set<Coordinate>();
    }
    _neighbors[a].insert(b);

    _coordinate_bucket_access_mutexes[mutex_index].unlock();
}

bool Graph::has_edge(const Coordinate &a, const Coordinate &b) const {
    return (_neighbors.find(a) != _neighbors.end()) &&
        (_neighbors.at(a).find(b) != _neighbors.at(a).end());
}

std::string Graph::to_string_representation() const {
    auto outs = std::stringstream();

    outs << "Graph (\n";
    for (const auto& neighbors: _neighbors) {
        outs << fmt::format("\t[({}, {}) [", neighbors.first.get_longitude(), neighbors.first.get_latitude());
        for (const auto& neighbor: neighbors.second) {
            outs << fmt::format("({}, {}) ", neighbor.get_longitude(), neighbor.get_latitude());
        }
        outs << "]]\n";
    }
    outs << ")";

    return outs.str();
}

bool Graph::operator==(const Graph& other) const {
    return _neighbors == other._neighbors && _polygons == other._polygons;
}

bool Graph::operator!=(const Graph &other) const {
    return !(*this == other);
}

void Graph::serialize_to_file(const std::string& path) const {
    const auto num_polygons = _polygons.size();
    const auto num_vertices = _neighbors.size();
    auto vertices = std::vector<Coordinate>();
    vertices.reserve(num_vertices);

    for (const auto& polygon: _polygons) {
        for (const auto& vertex: polygon.get_vertices()) {
            vertices.push_back(vertex);
        }
    }

    auto adjacency_matrix = std::vector<std::vector<bool>>(num_vertices, std::vector<bool>(num_vertices, false));
    for (size_t i = 0; i < num_vertices; ++i) {
        for (size_t j = 0; j < num_vertices; ++j) {
            const auto vertex_i = vertices[i];
            const auto vertex_j = vertices[j];

            if (_neighbors.at(vertex_i).find(vertex_j) != _neighbors.at(vertex_i).end()) {
                adjacency_matrix[i][j] = true;
            }
        }
    }

    auto file_stream = std::ofstream(path);
    if (file_stream.is_open()) {
        file_stream.write((char *) &num_polygons, sizeof(num_polygons));
        file_stream.flush();
        for (const auto& polygon: _polygons) {
            const auto num_vertices_in_polygon = polygon.get_vertices().size();
            file_stream.write((char *)&num_vertices_in_polygon, sizeof(num_vertices_in_polygon));
            file_stream.flush();
            for (const auto &vertex : polygon.get_vertices()) {
                const auto longitude = vertex.get_longitude();
                const auto latitude = vertex.get_latitude();

                file_stream.write((char *)&longitude, sizeof(longitude));
                file_stream.write((char *)&latitude, sizeof(latitude));
                file_stream.flush();
            }
        }

        for (size_t i = 0; i < num_vertices; ++i) {
            for (size_t j = 0; j <= i / 8 + (i % 8 != 0); ++j) {
                uint8_t adjacency_encoding = 0x0;

                for (size_t l = j*8; l <= std::min((j+1)*8 - 1, i); ++l) {
                    adjacency_encoding |= (static_cast<uint8_t>(adjacency_matrix[i][l]) << (l - j*8));
                }

                file_stream << static_cast<char>(adjacency_encoding);
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
        size_t num_polygons;
        if (!file_stream.read((char *) &num_polygons, sizeof(num_polygons))) {
            throw std::runtime_error("Num polygon header not present");
        }

        auto all_vertices = std::vector<Coordinate>();
        auto polygons = std::vector<Polygon>();
        polygons.reserve(num_polygons);
        for (size_t i = 0; i < num_polygons; ++i) {
            size_t num_vertices = 0;
            if (!file_stream.read((char *) &num_vertices, sizeof(num_vertices))) {
                throw std::runtime_error(fmt::format("{}th polygon missing num vertices", i));
            }

            auto vertices = std::vector<Coordinate>();
            vertices.reserve(num_vertices);
            for (size_t j = 0; j < num_vertices; ++j) {
                double longitude, latitude;
                if (!(file_stream.read((char *) &longitude, sizeof(longitude))) ||
                    !(file_stream.read((char *) &latitude, sizeof(latitude)))) {
                    throw std::runtime_error(fmt::format("{}th vertex of {}th polygon not found", j, i));
                }

                const auto coord = Coordinate(longitude, latitude);
                vertices.push_back(coord);
                all_vertices.push_back(coord);
            }

            polygons.emplace_back(vertices);
        }

        auto graph = Graph(polygons);

        for (size_t i = 0; i < all_vertices.size(); ++i) {
            for (size_t j = 0; j <= i / 8 + (i % 8 != 0); ++j) {
                uint8_t encoded_adjacency;
                if (!(file_stream.read((char *) &encoded_adjacency, sizeof(encoded_adjacency)))) {
                    throw std::runtime_error(fmt::format("{}th row {}th column adjacency matrix entry not found", i, j*8));
                }

                for (int l = 0; l < 8; ++l) {
                    if ((encoded_adjacency >> l) & 0x1) {
                        graph.add_edge(all_vertices[i], all_vertices[j*8 + l]);
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

std::vector<Coordinate> Graph::shortest_path(const Coordinate& source, const Coordinate& destination) const {
    auto modified_graph = Graph(*this);
    if (modified_graph._neighbors.find(source) == modified_graph._neighbors.end()) {
        for (const auto& visible_vertex : VistreeGenerator::get_visible_vertices_from_root(source, modified_graph._polygons, false)) {
            modified_graph.add_edge(source, visible_vertex);
        }
    }

    if (modified_graph._neighbors.find(destination) == modified_graph._neighbors.end()) {
        for (const auto& visible_vertex : VistreeGenerator::get_visible_vertices_from_root(destination, modified_graph._polygons, false)) {
            modified_graph.add_edge(destination, visible_vertex);
        }
    }

    const auto distance_measurement = [](const Coordinate& a, const Coordinate& b) {
        return (a - b).magnitude();
    };

    const auto comparison_func = [&](const AStarHeapElement& a, const AStarHeapElement& b) {
        return (a.distance_to_source + a.heuristic_distance_to_destination) > (b.distance_to_source + b.heuristic_distance_to_destination);
    };
    auto pq = std::priority_queue<AStarHeapElement, std::vector<AStarHeapElement>, decltype(comparison_func)>(comparison_func);
    pq.push(AStarHeapElement{
        .node = source,
        .distance_to_source = 0,
        .heuristic_distance_to_destination = distance_measurement(source, destination),
    });

    auto prev_coord = std::unordered_map<Coordinate, Coordinate>();
    auto distances_to_source = std::unordered_map<Coordinate, double>();
    while (!pq.empty()) {
        const auto top = pq.top(); pq.pop();

        if (top.node == destination) {
            break;
        }

        for (const auto& neighbor : modified_graph._neighbors.at(top.node)) {
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

std::ostream& operator<<(std::ostream& outs, const Graph& graph) {
    return outs << graph.to_string_representation();
}
