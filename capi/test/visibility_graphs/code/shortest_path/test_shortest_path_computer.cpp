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
    REQUIRE(shortest_path_ab == std::vector<Coordinate>{Coordinate(176., 0.),
                                                        Coordinate(1., 1.)});
}

TEST_CASE("ShortestPathComputer shortest path parallel to edge") {
    const auto poly = Polygon({
        Coordinate(1000, 1000),
        Coordinate(-1000, 1000),
        Coordinate(-1000, -1000),
        Coordinate(1000, -1000),
    });

    const auto graph = VisgraphGenerator::generate({poly});
    const auto a = Coordinate(-800, -1000);
    const auto b = Coordinate(800, -1000);
    const auto path_computer = ShortestPathComputer(graph);

    const auto shortest_path_ab = path_computer.shortest_path(a, b, INFINITY, true);
    auto shortest_path_ba = path_computer.shortest_path(b, a, INFINITY, true);
    std::reverse(shortest_path_ba.begin(), shortest_path_ba.end());

    REQUIRE(shortest_path_ab == shortest_path_ba);
    REQUIRE(shortest_path_ab == std::vector<Coordinate>{a, b});
}

TEST_CASE("ShortestPathComputer shortest paths") {
    const auto poly = Polygon({
        Coordinate(1., 0.),
        Coordinate(0., 1.),
        Coordinate(-1., 0.),
        Coordinate(0., -1.),
    });

    const auto graph = VisgraphGenerator::generate({poly});
    const auto a1 = Coordinate(-1.5, 0.5);
    const auto a2 = Coordinate(-1.5, -0.5);
    const auto a3 = Coordinate(0., 0.);

    const auto b1 = Coordinate(1.5, 0.);

    const auto source_dest_pairs = std::vector<std::pair<Coordinate, Coordinate>> {
        std::make_pair(a1, b1),
        std::make_pair(a2, b1),
        std::make_pair(a3, b1),
    };
    const auto path_computer = ShortestPathComputer(graph);

    const auto shortest_paths = path_computer.shortest_paths(source_dest_pairs);
    const auto expected_shortest_paths = std::vector<std::optional<std::vector<Coordinate>>>{
        std::make_optional(std::vector<Coordinate> {a1, Coordinate(0., 1.), b1}),
        std::make_optional(std::vector<Coordinate> {a2, Coordinate(0., -1.), b1}),
        std::nullopt,
    };

    REQUIRE(shortest_paths.size() == expected_shortest_paths.size());
    for (size_t i = 0; i < expected_shortest_paths.size(); ++i) {
        REQUIRE(shortest_paths[i].path == expected_shortest_paths[i]);

        if (expected_shortest_paths[i] == std::nullopt) {
            REQUIRE_FALSE(shortest_paths[i].error_msg == std::nullopt);
        } else{
            REQUIRE(shortest_paths[i].error_msg == std::nullopt);
        }
    }
}

TEST_CASE("ShortestPathComputer shortest path in land") {
    const auto poly = Polygon({
        Coordinate(1., 1.),
        Coordinate(1., -1.),
        Coordinate(-1., -1.),
        Coordinate(-1., 1.),
    });

    const auto graph = VisgraphGenerator::generate({poly});
    const auto a = Coordinate(0., 0.);
    const auto b = Coordinate(2., 0.);
    const auto c = Coordinate(0.5, 0.);
    const auto d = Coordinate(-2., 0.);
    const auto path_computer = ShortestPathComputer(graph);

    REQUIRE_THROWS(path_computer.shortest_path(a, b));
    REQUIRE_THROWS(path_computer.shortest_path(a, c));
    REQUIRE_THROWS(path_computer.shortest_path(d, a));
}

TEST_CASE("ShortestPathComputer shortest path in land correction") {
    const auto poly = Polygon({
                                      Coordinate(1., 1.),
                                      Coordinate(1., -1.),
                                      Coordinate(-1., -1.),
                                      Coordinate(-1., 1.),
                              });

    const auto graph = VisgraphGenerator::generate({poly});
    const auto a = Coordinate(-0.75, -0.0);
    const auto b = Coordinate(1.25, -0.1);
    const auto path_computer = ShortestPathComputer(graph);

    const auto path = path_computer.shortest_path(a, b, INFINITY, true);
    const auto expected_path = std::vector<Coordinate>{
        Coordinate(-1.,-0.0),
        Coordinate(-1.,-1.),
        Coordinate(1.,-1.),
        Coordinate(1.25,-0.1),
    };

    REQUIRE(path == expected_path);
}

TEST_CASE("ShortestPathComputer shortest path without obstacles") {
    const auto graph = VisgraphGenerator::generate({});
    const auto a = Coordinate(176., 0.);
    const auto b = Coordinate(1., 1.);
    const auto path_computer = ShortestPathComputer(graph);

    const auto shortest_path = path_computer.shortest_path(a, b);
    REQUIRE(shortest_path == std::vector<Coordinate>{a, b});
}

TEST_CASE("ShortestPathComputer prevent going out of bounds") {
    const auto poly = Polygon({
                                      Coordinate(1., 0.),
                                      Coordinate(0., 1.),
                                      Coordinate(-1., 0.),
                              });

    const auto graph = VisgraphGenerator::generate({poly});
    const auto a = Coordinate(-2., 0.);
    const auto b = Coordinate(1., 1.);
    const auto path_computer = ShortestPathComputer(graph);

    REQUIRE_THROWS(path_computer.shortest_path(a, b, 1.));
}
