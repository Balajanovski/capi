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
    void remove_edge(const Coordinate &a, const Coordinate &b);
    void remove_directed_edge(const Coordinate &a, const Coordinate &b);

    // This method is not thread-safe
    void add_vertex(const Coordinate &vertex);

    [[nodiscard]] bool has_vertex(const Coordinate &vertex) const;
    [[nodiscard]] bool has_edge(const Coordinate &a, const Coordinate &b) const;
    [[nodiscard]] bool is_edge_meridian_crossing(const Coordinate &a, const Coordinate &b) const;

    bool operator==(const Graph &other) const;
    bool operator!=(const Graph &other) const;

    [[nodiscard]] std::string to_string_representation() const;

    [[nodiscard]] bool are_adjacent(const Coordinate &vert1, const Coordinate &vert2) const;

    [[nodiscard]] std::vector<Coordinate> get_neighbors(const Coordinate &vertex) const;
    [[nodiscard]] std::vector<Coordinate> get_vertices() const;
    [[nodiscard]] std::vector<Polygon> get_polygons() const;

  private:
    Coordinate index_to_coordinate(unsigned int index) const;
    int coordinate_to_index(Coordinate coordinate) const;

    std::vector<Polygon> _polygons;
    tbb::concurrent_hash_map<unsigned int, std::unordered_map<unsigned int, bool>> _neighbors;
    std::unordered_map<Coordinate, unsigned int> _coordinate_to_index_mapping;
    std::vector<Coordinate> _index_to_coordinate_mapping;
};

Graph merge_graphs(const std::vector<Graph> &graphs);

std::ostream &operator<<(std::ostream &outs, const Graph &graph);

#endif // CAPI_GRAPH_HPP
