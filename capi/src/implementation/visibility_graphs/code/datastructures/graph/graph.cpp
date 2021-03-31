//
// Created by James.Balajan on 31/03/2021.
//

#include <stdexcept>
#include <fmt/core.h>

#include "graph.hpp"


Graph::Graph(int num_vertices): _neighbors(num_vertices, std::unordered_set<int>()), _num_vertices(num_vertices) {}

void Graph::add_edge(int a, int b) {
    if (a >= _num_vertices || a < 0 || b >= _num_vertices || b < 0) {
        throw std::runtime_error(fmt::format("Attempted to add edge ({}, {}) outside of allowed range. "
                                             "Num vertices: {}", a, b, _num_vertices));
    }

    _neighbors[a].insert(b);
    _neighbors[b].insert(a);
}

bool Graph::has_edge(int a, int b) const {
    if (a >= _num_vertices || a < 0 || b >= _num_vertices || b < 0) {
        return false;
    }

    return (_neighbors[a].find(b) != _neighbors[a].end());
}
