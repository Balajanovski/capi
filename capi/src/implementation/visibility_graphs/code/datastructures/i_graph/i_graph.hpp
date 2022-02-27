//
// Created by James.Balajan on 23-Feb.-2022.
//

#ifndef CAPI_I_GRAPH_HPP
#define CAPI_I_GRAPH_HPP

#include <string>

#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"

class IGraph {
public:
    virtual ~IGraph() = default;

    virtual void add_edge(const Coordinate &a, const Coordinate &b, bool meridian_crossing) = 0;
    virtual void add_directed_edge(const Coordinate &a, const Coordinate &b, bool meridian_crossing) = 0;
    virtual void remove_edge(const Coordinate &a, const Coordinate &b) = 0;
    virtual void remove_directed_edge(const Coordinate &a, const Coordinate &b) = 0;

    virtual void add_vertex(const Coordinate &vertex) = 0;

    [[nodiscard]] virtual bool has_vertex(const Coordinate &vertex) const = 0;
    [[nodiscard]] virtual bool has_edge(const Coordinate &a, const Coordinate &b) const = 0;
    [[nodiscard]] virtual bool is_edge_meridian_crossing(const Coordinate &a, const Coordinate &b) const = 0;

    [[nodiscard]] virtual std::vector<Coordinate> get_neighbors(const Coordinate &vertex) const = 0;
    [[nodiscard]] virtual std::vector<Coordinate> get_vertices() const = 0;
    [[nodiscard]] virtual std::vector<Polygon> get_polygons() const = 0;
};

#endif //CAPI_I_GRAPH_HPP
