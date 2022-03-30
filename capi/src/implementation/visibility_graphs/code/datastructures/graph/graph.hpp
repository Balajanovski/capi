//
// Created by James.Balajan on 31/03/2021.
//

#ifndef CAPI_GRAPH_HPP
#define CAPI_GRAPH_HPP

#include <array>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <shared_mutex>
#include <memory>

#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"
#include "datastructures/i_graph/i_graph.hpp"

class Graph : public IGraph {
  public:
    Graph();
    explicit Graph(std::vector<Polygon> polygons);

    void add_edge(const Coordinate &a, const Coordinate &b, bool meridian_crossing) override;
    void add_directed_edge(const Coordinate &a, const Coordinate &b, bool meridian_crossing) override;
    void remove_edge(const Coordinate &a, const Coordinate &b) override;
    void remove_directed_edge(const Coordinate &a, const Coordinate &b) override;

    // This method is not thread-safe
    void add_vertex(const Coordinate &vertex) override;

    [[nodiscard]] bool has_vertex(const Coordinate &vertex) const override;
    [[nodiscard]] bool has_edge(const Coordinate &a, const Coordinate &b) const override;
    [[nodiscard]] bool is_edge_meridian_crossing(const Coordinate &a, const Coordinate &b) const override;

    bool operator==(const Graph &other) const;
    bool operator!=(const Graph &other) const;

    [[nodiscard]] std::string to_string_representation() const;

    [[nodiscard]] std::vector<Coordinate> get_neighbors(const Coordinate &vertex) const override;
    [[nodiscard]] std::vector<Coordinate> get_vertices() const override;
    [[nodiscard]] std::vector<Polygon> get_polygons() const override;

  private:
    Coordinate index_to_coordinate(unsigned int index) const;
    unsigned int coordinate_to_index(Coordinate coordinate) const;

    std::vector<Polygon> _polygons;
    size_t _num_coords = 0;

    enum EdgeState : uint8_t {
        DISCONNECTED = 0x0,
        CONNECTED = 0x1,
        CONNECTED_OVER_MERIDIAN = 0x2,
        CONNECTED_BOTH = 0x3,
    };

    std::vector<EdgeState> _neighbors;
    mutable std::vector<std::unique_ptr<std::shared_mutex>> _accessor_locks;

    std::unordered_map<Coordinate, unsigned int> _coordinate_to_index_mapping;
    std::vector<Coordinate> _index_to_coordinate_mapping;
};

std::shared_ptr<Graph> merge_graphs(const std::vector<std::shared_ptr<Graph>> &graphs);

std::ostream &operator<<(std::ostream &outs, const Graph &graph);

#endif // CAPI_GRAPH_HPP
