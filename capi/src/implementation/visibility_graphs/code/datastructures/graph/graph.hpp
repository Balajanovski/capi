//
// Created by James.Balajan on 31/03/2021.
//

#ifndef CAPI_GRAPH_HPP
#define CAPI_GRAPH_HPP

#include <unordered_set>
#include <unordered_map>

#include "types/coordinate/coordinate.hpp"

class Graph {
    friend std::ostream& operator<<(std::ostream& outs, const Graph& graph);

  public:
    Graph();
    explicit Graph(size_t num_vertices);

    void add_edge(const Coordinate& a, const Coordinate& b);
    [[nodiscard]] bool has_edge(const Coordinate& a, const Coordinate& b) const;

    bool operator==(const Graph& other) const;
    bool operator!=(const Graph& other) const;

  private:
    std::unordered_map<Coordinate, std::unordered_set<Coordinate>> _neighbors;
};

std::ostream& operator<<(std::ostream& outs, const Graph& graph);

#endif // CAPI_GRAPH_HPP
