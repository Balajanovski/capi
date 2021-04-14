//
// Created by James.Balajan on 31/03/2021.
//

#include <fmt/core.h>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "graph.hpp"

Graph::Graph() = default;

Graph::Graph(const Graph& other_graph): _neighbors(other_graph._neighbors) {}

Graph::Graph(size_t num_vertices) {
    _neighbors.reserve(num_vertices);
}

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
    return _neighbors == other._neighbors;
}

bool Graph::operator!=(const Graph &other) const {
    return !(*this == other);
}

void Graph::serialize_to_file(const std::string& path) const {
    const auto num_vertices = _neighbors.size();
    auto vertices = std::vector<Coordinate>(num_vertices, Coordinate(0, 0));
    std::transform(_neighbors.begin(), _neighbors.end(), vertices.begin(), [](const auto& pair) { return pair.first; });

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
        file_stream << num_vertices << '\n';
        for (const auto& vertex: vertices) {
            file_stream << vertex.to_string_representation() << '\n';
        }

        for (size_t i = 0; i < num_vertices; ++i) {
            for (size_t j = 0; j <= i; ++j) {
                file_stream << (adjacency_matrix[i][j] ? '1' : '0');

                if (j < i) {
                    file_stream << ' ';
                }
            }

            file_stream << '\n';
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
        if (!getline(file_stream, file_read_str)) {
            throw std::runtime_error("Num vertex header not present");
        }
        const auto num_vertices = std::stoul(file_read_str);

        auto graph = Graph(num_vertices);
        auto vertices = std::vector<Coordinate>(num_vertices, Coordinate(0, 0));
        for (size_t i = 0; i < num_vertices; ++i) {
            if (!getline(file_stream, file_read_str)) {
                throw std::runtime_error(fmt::format("Vertex {} was not present", i));
            }
            vertices[i] = Coordinate::parse_from_string(file_read_str);
        }

        for (size_t i = 0; i < num_vertices; ++i) {
            for (size_t j = 0; j <= i; ++j) {
                if (!(file_stream >> file_read_str)) {
                    throw std::runtime_error(fmt::format("{}th row {}th column adjacency matrix entry not found", i, j));
                }

                const auto adjacency_matrix_entry = static_cast<bool>(std::stoi(file_read_str));
                if (adjacency_matrix_entry) {
                    graph.add_edge(vertices[i], vertices[j]);
                }
            }
        }

        file_stream.close();

        return graph;
    } else {
        throw std::runtime_error(fmt::format("Could not open file {}", path));
    }
}

std::ostream& operator<<(std::ostream& outs, const Graph& graph) {
    return outs << graph.to_string_representation();
}
