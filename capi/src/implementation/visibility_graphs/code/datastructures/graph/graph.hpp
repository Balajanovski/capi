//
// Created by James.Balajan on 31/03/2021.
//

#ifndef CAPI_GRAPH_HPP
#define CAPI_GRAPH_HPP

#include <unordered_set>
#include <unordered_map>
#include <mutex>
#include <array>
#include <string>

#include "types/coordinate/coordinate.hpp"

// TODO: GRAPH SERIALIZATION

class Graph {
  public:
    Graph();
    Graph(const Graph& other_graph);
    explicit Graph(size_t num_vertices);

    void add_edge(const Coordinate& a, const Coordinate& b);
    void add_directed_edge(const Coordinate& a, const Coordinate& b);
    [[nodiscard]] bool has_edge(const Coordinate& a, const Coordinate& b) const;

    bool operator==(const Graph& other) const;
    bool operator!=(const Graph& other) const;

    [[nodiscard]] std::string to_string_representation() const;
  private:
    static constexpr size_t NUM_COORDINATE_MUTEXES = 500;

    std::unordered_map<Coordinate, std::unordered_set<Coordinate>> _neighbors;
    std::array<std::mutex, NUM_COORDINATE_MUTEXES> _coordinate_bucket_access_mutexes;
};

std::ostream& operator<<(std::ostream& outs, const Graph& graph);

#endif // CAPI_GRAPH_HPP
