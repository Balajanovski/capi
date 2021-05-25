//
// Created by James.Balajan on 31/03/2021.
//

#ifndef CAPI_GRAPH_HPP
#define CAPI_GRAPH_HPP

#include <array>
#include <mutex>
#include <string>
#include <tbb/concurrent_hash_map.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"

class Graph {
  public:
    Graph();
    explicit Graph(std::vector<Polygon> polygons);
    Graph(const Graph &other_graph);

    void add_edge(const Coordinate &a, const Coordinate &b, bool meridian_crossing);
    void add_directed_edge(const Coordinate &a, const Coordinate &b, bool meridian_crossing);
    [[nodiscard]] bool has_edge(const Coordinate &a, const Coordinate &b) const;
    [[nodiscard]] bool is_edge_meridian_crossing(const Coordinate &a, const Coordinate &b) const;

    bool operator==(const Graph &other) const;
    bool operator!=(const Graph &other) const;

    [[nodiscard]] std::string to_string_representation() const;

    [[nodiscard]] std::vector<Coordinate> shortest_path(const Coordinate &source, const Coordinate &destination) const;

    [[nodiscard]] bool are_adjacent(const Coordinate &vert1, const Coordinate &vert2) const;

    [[nodiscard]] std::vector<Coordinate> get_neighbors(const Coordinate &vertex) const;
    [[nodiscard]] std::vector<Coordinate> get_vertices() const;
    [[nodiscard]] std::vector<Polygon> get_polygons() const;

  private:
    struct CoordinateHashCompare {
        static size_t hash(const Coordinate &c) { return std::hash<Coordinate>()(c); }

        static bool equal(const Coordinate &lhs, const Coordinate &rhs) { return lhs == rhs; }
    };

    tbb::concurrent_hash_map<Coordinate, std::unordered_map<Coordinate, bool>, CoordinateHashCompare> _neighbors;
    std::vector<Polygon> _polygons;
};

Graph merge_graphs(const std::vector<Graph> &graphs);

std::ostream &operator<<(std::ostream &outs, const Graph &graph);

#endif // CAPI_GRAPH_HPP
