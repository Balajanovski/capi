//
// Created by James.Balajan on 31/03/2021.
//

#include <algorithm>
#include <catch.hpp>

#include "datastructures/graph/graph.hpp"
#include "visgraph/visgraph_generator.hpp"

TEST_CASE("Graph Add Edge") {
    const auto coord1 = Coordinate(1., 2.);
    const auto coord2 = Coordinate(2., 1.);
    const auto coord3 = Coordinate(1., 1.);
    const auto coord4 = Coordinate(1., 5.);

    auto graph = Graph(std::vector<Polygon>{Polygon({coord1, coord2, coord3, coord4})});

    graph.add_edge(coord1, coord2, true);
    graph.add_edge(coord1, coord2, true);

    graph.add_edge(coord3, coord4, true);
    graph.add_edge(coord3, coord4, false);

    REQUIRE(graph.has_edge(coord1, coord2));
    REQUIRE(graph.has_edge(coord2, coord1));
    REQUIRE_FALSE(graph.has_edge(coord1, coord3));

    REQUIRE(graph.is_edge_meridian_crossing(coord1, coord2));
    REQUIRE_FALSE(graph.is_edge_meridian_crossing(coord3, coord4));
    REQUIRE_FALSE(graph.is_edge_meridian_crossing(coord1, coord3));
}

TEST_CASE("Graph Remove Edge") {
    const auto coord1 = Coordinate(1., 2.);
    const auto coord2 = Coordinate(2., 1.);
    const auto coord3 = Coordinate(3., 1.);

    auto graph = Graph(std::vector<Polygon>{Polygon({coord1, coord2, coord3})});

    graph.add_edge(coord1, coord2, true);
    graph.remove_edge(coord1, coord2);
    graph.remove_edge(coord1, coord3);

    REQUIRE_FALSE(graph.has_edge(coord1, coord2));
    REQUIRE_FALSE(graph.has_edge(coord2, coord1));
}

TEST_CASE("Graph get_vertices") {
    const auto coord1 = Coordinate(1., 2.);
    const auto coord2 = Coordinate(2., 1.);
    const auto coord3 = Coordinate(1., 1.);

    auto graph = Graph(std::vector<Polygon>{Polygon({coord1, coord2, coord3})});

    REQUIRE(std::find(graph.get_vertices().begin(), graph.get_vertices().end(), coord1) != graph.get_vertices().end());
    REQUIRE(std::find(graph.get_vertices().begin(), graph.get_vertices().end(), coord2) != graph.get_vertices().end());
    REQUIRE(std::find(graph.get_vertices().begin(), graph.get_vertices().end(), coord3) != graph.get_vertices().end());
}

TEST_CASE("Graph get_neighbors") {
    const auto coord1 = Coordinate(1., 2.);
    const auto coord2 = Coordinate(2., 1.);
    const auto coord3 = Coordinate(1., 1.);

    auto graph = Graph(std::vector<Polygon>{Polygon({coord1, coord2, coord3})});

    graph.add_edge(coord1, coord2, false);

    REQUIRE(graph.get_neighbors(coord1) == std::vector<Coordinate>{coord2});
    REQUIRE(graph.get_neighbors(coord2) == std::vector<Coordinate>{coord1});
}

TEST_CASE("Graph get_polygons") {
    const auto coord1 = Coordinate(1., 2.);
    const auto coord2 = Coordinate(2., 1.);
    const auto coord3 = Coordinate(1., 1.);
    const auto polygon = Polygon({coord1, coord2, coord3});
    const auto polygons = std::vector<Polygon>{polygon};

    auto graph = Graph(polygons);

    REQUIRE(graph.get_polygons() == polygons);
}

TEST_CASE("Graph merge") {
    const auto poly1 = Polygon({
        Coordinate(1., 0.),
        Coordinate(0., 1.5),
        Coordinate(-1., 0.),
    });

    const auto poly2 = Polygon({
        Coordinate(4., 0.),
        Coordinate(3., 1.),
        Coordinate(2., 0.),
    });

    const auto single_graph = VisgraphGenerator::generate({poly1, poly2});
    const auto graph_split_1 = VisgraphGenerator::generate_with_shuffled_range({poly1, poly2}, 0, 3, 0);
    const auto graph_split_2 = VisgraphGenerator::generate_with_shuffled_range({poly1, poly2}, 3, 6, 0);
    const auto merged_graph = merge_graphs({graph_split_1, graph_split_2});

    REQUIRE(*single_graph == *merged_graph);
}
