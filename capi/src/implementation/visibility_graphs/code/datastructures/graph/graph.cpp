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
