//
// Created by James.Balajan on 31/03/2021.
//

#include <fmt/core.h>
#include <stdexcept>

#include "graph.hpp"

Graph::Graph() = default;

Graph::Graph(size_t num_vertices) {
    _neighbors.reserve(num_vertices);
}

void Graph::add_edge(const Coordinate &a, const Coordinate &b) {
    if (_neighbors.find(a) == _neighbors.end()) {
        _neighbors[a] = std::unordered_set<Coordinate>();
    } if (_neighbors.find(b) == _neighbors.end()) {
        _neighbors[b] = std::unordered_set<Coordinate>();
    }

    _neighbors[a].insert(b);
    _neighbors[b].insert(a);
}

bool Graph::has_edge(const Coordinate &a, const Coordinate &b) const {
    return (_neighbors.find(a) != _neighbors.end()) &&
        (_neighbors.at(a).find(b) != _neighbors.at(a).end());
}

std::ostream& operator<<(std::ostream& outs, const Graph& graph) {
    outs << "Graph (\n";
    for (const auto& neighbors: graph._neighbors) {
        outs << fmt::format("\t[({}, {}) [", neighbors.first.get_longitude(), neighbors.first.get_latitude());
        for (const auto& neighbor: neighbors.second) {
            outs << fmt::format("({}, {}) ", neighbor.get_longitude(), neighbor.get_latitude());
        }
        outs << "]]\n";
    }
    outs << ")";

    return outs;
}

bool Graph::operator==(const Graph& other) const {
    return _neighbors == other._neighbors;
}

bool Graph::operator!=(const Graph &other) const {
    return !(*this == other);
}
