//
// Created by James.Balajan on 12/04/2021.
//

#include <catch.hpp>
#include <vector>
#include <iostream>

#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"
#include "visgraph/visgraph_generator.hpp"

TEST_CASE("Visgraph Generator") {
    const auto poly1 = Polygon({
                                       Coordinate(1, 0),
                                       Coordinate(0, 1),
                                       Coordinate(-1, 0),
                                       Coordinate(-1, -1),
                                       Coordinate(0, -1),
                                       Coordinate(0.3, -0.5),
                               });

    const auto poly2 = Polygon({
                                       Coordinate(3, -1),
                                       Coordinate(2, -2),
                                       Coordinate(2.9, -3),
                                       Coordinate(3, -3),
                                       Coordinate(4, -2),
                               });

    const auto visgraph = VisgraphGenerator::generate(std::vector<Polygon> {poly1, poly2});

    auto expectedVisGraph = Graph(11);
    expectedVisGraph.add_edge(Coordinate(4, -2), Coordinate(3, -3));
    expectedVisGraph.add_edge(Coordinate(4, -2), Coordinate(3, -1));

    expectedVisGraph.add_edge(Coordinate(3, -3), Coordinate(4, -2));
    expectedVisGraph.add_edge(Coordinate(3, -3), Coordinate(2.9, -3));

    expectedVisGraph.add_edge(Coordinate(2.9, -3), Coordinate(-1, 0));
    expectedVisGraph.add_edge(Coordinate(2.9, -3), Coordinate(-1, -1));
    expectedVisGraph.add_edge(Coordinate(2.9, -3), Coordinate(0, -1));
    expectedVisGraph.add_edge(Coordinate(2.9, -3), Coordinate(2, -2));
    expectedVisGraph.add_edge(Coordinate(2.9, -3), Coordinate(0.3, -0.5));

    expectedVisGraph.add_edge(Coordinate(-1, -1), Coordinate(-1, 0));
    expectedVisGraph.add_edge(Coordinate(-1, -1), Coordinate(4, -2));
    expectedVisGraph.add_edge(Coordinate(-1, -1), Coordinate(2, -2));
    expectedVisGraph.add_edge(Coordinate(-1, -1), Coordinate(2.9, -3));
    expectedVisGraph.add_edge(Coordinate(-1, -1), Coordinate(3, -3));

    expectedVisGraph.add_edge(Coordinate(3, -1), Coordinate(2, -2));
    expectedVisGraph.add_edge(Coordinate(3, -1), Coordinate(0, -1));
    expectedVisGraph.add_edge(Coordinate(3, -1), Coordinate(0, 1));
    expectedVisGraph.add_edge(Coordinate(3, -1), Coordinate(4, -2));
    expectedVisGraph.add_edge(Coordinate(3, -1), Coordinate(0.3, -0.5));
    expectedVisGraph.add_edge(Coordinate(3, -1), Coordinate(1, 0));

    expectedVisGraph.add_edge(Coordinate(2, -2), Coordinate(2.9, -3));
    expectedVisGraph.add_edge(Coordinate(2, -2), Coordinate(3, -1));
    expectedVisGraph.add_edge(Coordinate(2, -2), Coordinate(0, -1));
    expectedVisGraph.add_edge(Coordinate(2, -2), Coordinate(-1, -1));
    expectedVisGraph.add_edge(Coordinate(2, -2), Coordinate(1, 0));
    expectedVisGraph.add_edge(Coordinate(2, -2), Coordinate(0, 1));
    expectedVisGraph.add_edge(Coordinate(2, -2), Coordinate(0.3, -0.5));
    expectedVisGraph.add_edge(Coordinate(2, -2), Coordinate(-1, 0));

    expectedVisGraph.add_edge(Coordinate(-1, 0), Coordinate(0, 1));
    expectedVisGraph.add_edge(Coordinate(-1, 0), Coordinate(-1, -1));

    expectedVisGraph.add_edge(Coordinate(0.3, -0.5), Coordinate(1, 0));
    expectedVisGraph.add_edge(Coordinate(0.3, -0.5), Coordinate(3, -1));
    expectedVisGraph.add_edge(Coordinate(0.3, -0.5), Coordinate(2, -2));
    expectedVisGraph.add_edge(Coordinate(0.3, -0.5), Coordinate(0, -1));
    expectedVisGraph.add_edge(Coordinate(0.3, -0.5), Coordinate(2.9, -3));

    expectedVisGraph.add_edge(Coordinate(0, -1), Coordinate(0.3, -0.5));
    expectedVisGraph.add_edge(Coordinate(0, -1), Coordinate(1, 0));
    expectedVisGraph.add_edge(Coordinate(0, -1), Coordinate(3, -3));
    expectedVisGraph.add_edge(Coordinate(0, -1), Coordinate(2, -2));
    expectedVisGraph.add_edge(Coordinate(0, -1), Coordinate(-1, -1));
    expectedVisGraph.add_edge(Coordinate(0, -1), Coordinate(2.9, -3));

    expectedVisGraph.add_edge(Coordinate(0, 1), Coordinate(3, -1));
    expectedVisGraph.add_edge(Coordinate(0, 1), Coordinate(-1, 0));
    expectedVisGraph.add_edge(Coordinate(0, 1), Coordinate(1, 0));

    expectedVisGraph.add_edge(Coordinate(1, 0), Coordinate(3, -1));
    expectedVisGraph.add_edge(Coordinate(1, 0), Coordinate(2, -2));
    expectedVisGraph.add_edge(Coordinate(1, 0), Coordinate(0, -1));
    expectedVisGraph.add_edge(Coordinate(1, 0), Coordinate(0, 1));
    expectedVisGraph.add_edge(Coordinate(1, 0), Coordinate(0.3, -0.5));

    REQUIRE(visgraph == expectedVisGraph);
}
