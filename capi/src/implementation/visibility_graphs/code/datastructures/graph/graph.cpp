//
// Created by James.Balajan on 31/03/2021.
//

#include <fmt/core.h>
#include <stdexcept>
#include <sstream>

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

std::ostream& operator<<(std::ostream& outs, const Graph& graph) {
    return outs << graph.to_string_representation();
}
