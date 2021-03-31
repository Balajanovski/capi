//
// Created by James.Balajan on 31/03/2021.
//

#include <catch.hpp>
#include <stdexcept>

#include "datastructures/graph/graph.hpp"


TEST_CASE("Graph Add Edge")
{
    auto graph = Graph(10);
    graph.add_edge(1, 5);

    REQUIRE(graph.has_edge(1, 5));
    REQUIRE(graph.has_edge(5, 1));
    REQUIRE_FALSE(graph.has_edge(1, 6));
}

TEST_CASE("Graph edge outside allowed range")
{
    auto graph = Graph(0);

    REQUIRE_FALSE(graph.has_edge(1, 0));
    REQUIRE_THROWS_AS(graph.add_edge(1, 0), std::runtime_error);
}
