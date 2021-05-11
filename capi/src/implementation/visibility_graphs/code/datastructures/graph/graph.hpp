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
#include <vector>

#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"

class Graph {
  public:
    explicit Graph(const std::vector<Polygon>& polygons);
    Graph(const Graph& other_graph);

    void add_edge(const Coordinate& a, const Coordinate& b);
    void add_directed_edge(const Coordinate& a, const Coordinate& b);
    [[nodiscard]] bool has_edge(const Coordinate& a, const Coordinate& b) const;

    bool operator==(const Graph& other) const;
    bool operator!=(const Graph& other) const;

    [[nodiscard]] std::string to_string_representation() const;

    void serialize_to_file(const std::string& path) const;
    static Graph load_from_file(const std::string& path);

    std::vector<Coordinate> shortest_path(const Coordinate& source, const Coordinate& destination) const;
  private:
    static constexpr size_t NUM_COORDINATE_MUTEXES = 500;
    std::array<std::mutex, NUM_COORDINATE_MUTEXES> _coordinate_bucket_access_mutexes;

    std::unordered_map<Coordinate, std::unordered_set<Coordinate>> _neighbors;
    std::vector<Polygon> _polygons;
};

std::ostream& operator<<(std::ostream& outs, const Graph& graph);

#endif // CAPI_GRAPH_HPP
