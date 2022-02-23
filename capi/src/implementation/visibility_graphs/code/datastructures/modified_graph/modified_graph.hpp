//
// Created by James.Balajan on 23-Feb.-2022.
//

#ifndef CAPI_MODIFIED_GRAPH_HPP
#define CAPI_MODIFIED_GRAPH_HPP

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "types/coordinate/coordinate.hpp"
#include "datastructures/i_graph/i_graph.hpp"

namespace {
    enum NeighborRelationship {
        EDGE_PRESENT = 0x1,
        EDGE_REMOVED = 0x2,
    };

    struct NeighborInfo {
        bool is_meridian_crossing;
        NeighborRelationship relationship;
    };
}

// Unlike graph, modified graph is not thread-safe
// Treat with caution
class ModifiedGraph : public IGraph {
    public:
        explicit ModifiedGraph(std::shared_ptr<IGraph> base_graph);

        void add_edge(const Coordinate &a, const Coordinate &b, bool meridian_crossing) override;
        void add_directed_edge(const Coordinate &a, const Coordinate &b, bool meridian_crossing) override;
        void remove_edge(const Coordinate &a, const Coordinate &b) override;
        void remove_directed_edge(const Coordinate &a, const Coordinate &b) override;

        void add_vertex(const Coordinate &vertex) override;

        [[nodiscard]] bool has_vertex(const Coordinate &vertex) const override;
        [[nodiscard]] bool has_edge(const Coordinate &a, const Coordinate &b) const override;
        [[nodiscard]] bool is_edge_meridian_crossing(const Coordinate &a, const Coordinate &b) const override;

        [[nodiscard]] std::vector<Coordinate> get_neighbors(const Coordinate &vertex) const override;
        [[nodiscard]] std::vector<Coordinate> get_vertices() const override;
        [[nodiscard]] std::vector<Polygon> get_polygons() const override;
    private:
        std::shared_ptr<IGraph> _graph;
        std::unordered_map<Coordinate, std::unordered_map<Coordinate, NeighborInfo>> _neighbors;
};

#endif //CAPI_MODIFIED_GRAPH_HPP
