//
// Created by James.Balajan on 31/03/2021.
//

#include <catch.hpp>
#include <stdexcept>
#include <cstdio>

#include "types/coordinate/coordinate.hpp"
#include "datastructures/graph/graph.hpp"

TEST_CASE("Graph Add Edge") {
    auto graph = Graph();
    const auto coord1 = Coordinate(1, 2);
    const auto coord2 = Coordinate(2, 1);
    const auto coord3 = Coordinate(1, 1);
    const auto coord4 = Coordinate(1, 2);

    graph.add_edge(coord1, coord2);

    REQUIRE(graph.has_edge(coord1, coord2));
    REQUIRE(graph.has_edge(coord2, coord1));
    REQUIRE_FALSE(graph.has_edge(coord1, coord3));
    REQUIRE(graph.has_edge(coord4, coord2));
}

TEST_CASE("Graph serialize") {
    auto graph = Graph();

    const auto coord1 = Coordinate(1, 2);
    const auto coord2 = Coordinate(2, 1);
    const auto coord3 = Coordinate(1, 1);
    const auto coord4 = Coordinate(10, 2);

    graph.add_edge(coord1, coord2);
    graph.add_edge(coord1, coord3);
    graph.add_edge(coord3, coord4);

    char tmp_name[L_tmpnam];
    tmpnam(tmp_name);

    graph.serialize_to_file(tmp_name);
    const auto deserialized_graph = Graph::load_from_file(tmp_name);

    remove(tmp_name);

    REQUIRE(graph == deserialized_graph);
}
