//
// Created by James.Balajan on 31/03/2021.
//

#include <catch.hpp>
#include <stdexcept>
#include <cstdio>
#include <algorithm>

#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"
#include "datastructures/graph/graph.hpp"
#include "visgraph/visgraph_generator.hpp"

TEST_CASE("Graph Add Edge") {
    const auto coord1 = Coordinate(1, 2);
    const auto coord2 = Coordinate(2, 1);
    const auto coord3 = Coordinate(1, 1);
    const auto coord4 = Coordinate(1, 2);

    auto graph = Graph(std::vector<Polygon>{Polygon({coord1, coord2, coord3})});

    graph.add_edge(coord1, coord2);

    REQUIRE(graph.has_edge(coord1, coord2));
    REQUIRE(graph.has_edge(coord2, coord1));
    REQUIRE_FALSE(graph.has_edge(coord1, coord3));
    REQUIRE(graph.has_edge(coord4, coord2));
}

TEST_CASE("Graph get_vertices") {
    const auto coord1 = Coordinate(1, 2);
    const auto coord2 = Coordinate(2, 1);
    const auto coord3 = Coordinate(1, 1);

    auto graph = Graph(std::vector<Polygon>{Polygon({coord1, coord2, coord3})});

    REQUIRE(std::find(graph.get_vertices().begin(), graph.get_vertices().end(), coord1) != graph.get_vertices().end());
    REQUIRE(std::find(graph.get_vertices().begin(), graph.get_vertices().end(), coord2) != graph.get_vertices().end());
    REQUIRE(std::find(graph.get_vertices().begin(), graph.get_vertices().end(), coord3) != graph.get_vertices().end());
}

TEST_CASE("Graph get_neighbors") {
    const auto coord1 = Coordinate(1, 2);
    const auto coord2 = Coordinate(2, 1);
    const auto coord3 = Coordinate(1, 1);

    auto graph = Graph(std::vector<Polygon>{Polygon({coord1, coord2, coord3})});

    graph.add_edge(coord1, coord2);

    REQUIRE(graph.get_neighbors(coord1) == std::vector<Coordinate>{coord2});
    REQUIRE(graph.get_neighbors(coord2) == std::vector<Coordinate>{coord1});
}

TEST_CASE("Graph get_polygons") {
    const auto coord1 = Coordinate(1, 2);
    const auto coord2 = Coordinate(2, 1);
    const auto coord3 = Coordinate(1, 1);
    const auto polygon = Polygon({coord1, coord2, coord3});
    const auto polygons = std::vector<Polygon>{polygon};

    auto graph = Graph(polygons);

    REQUIRE(graph.get_polygons() == polygons);
}

TEST_CASE("Graph shortest path") {
    const auto poly1 = Polygon({
                                   Coordinate(1, 0),
                                   Coordinate(0, 1.5),
                                   Coordinate(-1, 0),
                               });

    const auto poly2 = Polygon({
                                   Coordinate(4, 0),
                                   Coordinate(3, 1),
                                   Coordinate(2, 0),
                               });

    const auto graph = VisgraphGenerator::generate({poly1, poly2});
    const auto a = Coordinate(-2, 0);
    const auto b = Coordinate(3, 1);

    const auto shortest_path_ab = graph.shortest_path(a, b);
    auto shortest_path_ba = graph.shortest_path(b, a);
    std::reverse(shortest_path_ba.begin(), shortest_path_ba.end());

    REQUIRE(shortest_path_ab == shortest_path_ba);
    REQUIRE(shortest_path_ab == std::vector<Coordinate>{ Coordinate(-2, 0), Coordinate(-1, 0), Coordinate(1, 0), Coordinate(3, 1) });
}