//
// Created by James.Balajan on 3/06/2021.
//

#include <catch.hpp>

#include "datastructures/graph/graph.hpp"
#include "shortest_path/shortest_path_computer.hpp"
#include "visgraph/visgraph_generator.hpp"

TEST_CASE("ShortestPathComputer shortest path") {
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

    const auto graph = VisgraphGenerator::generate({poly1, poly2});
    const auto a = Coordinate(-2., 0.);
    const auto b = Coordinate(3., 1.);
    const auto path_computer = ShortestPathComputer(graph);

    const auto shortest_path_ab = path_computer.shortest_path(a, b);
    auto shortest_path_ba = path_computer.shortest_path(b, a);
    std::reverse(shortest_path_ba.begin(), shortest_path_ba.end());

    REQUIRE(shortest_path_ab == shortest_path_ba);
    REQUIRE(shortest_path_ab ==
            std::vector<Coordinate>{Coordinate(-2., 0.), Coordinate(-1., 0.), Coordinate(1., 0.), Coordinate(3., 1.)});
}

TEST_CASE("ShortestPathComputer shortest path over meridian") {
    const auto poly1 = Polygon({
                                   Coordinate(179., 0.),
                                   Coordinate(178., 1.5),
                                   Coordinate(177., 0.),
                               });

    const auto poly2 = Polygon({
                                   Coordinate(2., 0.),
                                   Coordinate(1., 1.),
                                   Coordinate(0., 0.),
                               });

    const auto graph = VisgraphGenerator::generate({poly1, poly2});
    const auto a = Coordinate(176., 0.);
    const auto b = Coordinate(1., 1.);
    const auto path_computer = ShortestPathComputer(graph);

    const auto shortest_path_ab = path_computer.shortest_path(a, b);
    auto shortest_path_ba = path_computer.shortest_path(b, a);
    std::reverse(shortest_path_ba.begin(), shortest_path_ba.end());

    REQUIRE(shortest_path_ab == shortest_path_ba);
    REQUIRE(shortest_path_ab == std::vector<Coordinate>{Coordinate(176., 0.), Coordinate(1., 1.)});
}
