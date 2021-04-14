//
// Created by James.Balajan on 12/04/2021.
//

#include <catch.hpp>
#include <vector>
#include <iostream>

#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"
#include "visgraph/visgraph_generator.hpp"

void add_edges(const Coordinate& source, const std::vector<Coordinate>& neighbors, Graph& g);

TEST_CASE("Visgraph Generator Normal Case") {
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

    auto expectedVisGraph = Graph({poly1, poly2});
    add_edges(Coordinate(3, -1),
            {
        Coordinate(4, -2),
        Coordinate(1, 0),
        Coordinate(0.3, -0.5),
        Coordinate(0, 1),
        Coordinate(-1, -1),
        Coordinate(0, -1),
        Coordinate(2, -2),
        },
        expectedVisGraph);
    add_edges(Coordinate(-1, -1),
              {
        Coordinate(-1, 0),
        Coordinate(3, -1),
        Coordinate(0, -1),
        Coordinate(2.9, -3),
        Coordinate(2, -2),
              }, expectedVisGraph);
    add_edges(Coordinate(2, -2),
              {
        Coordinate(3, -1),
        Coordinate(2.9, -3),
        Coordinate(-1, -1),
        Coordinate(0, -1),
        Coordinate(1, 0),
        Coordinate(0.3, -0.5),
              }, expectedVisGraph);
    add_edges(Coordinate(0, 1),
              {
        Coordinate(3, -1),
        Coordinate(-1, 0),
        Coordinate(1, 0),
              }, expectedVisGraph);
    add_edges(Coordinate(0, -1),
              {
        Coordinate(0.3, -0.5),
        Coordinate(1, 0),
        Coordinate(3, -1),
        Coordinate(2, -2),
        Coordinate(-1, -1),
        Coordinate(2.9, -3),
              }, expectedVisGraph);
    add_edges(Coordinate(0.3, -0.5),
              {
        Coordinate(3, -1),
        Coordinate(2.9, -3),
        Coordinate(2, -2),
              }, expectedVisGraph);
    add_edges(Coordinate(-1, 0),
              {
        Coordinate(0, 1),
        Coordinate(-1, -1),
              }, expectedVisGraph);
    add_edges(Coordinate(3, -3),
              {
        Coordinate(4, -2),
        Coordinate(2.9, -3),
              }, expectedVisGraph);
    add_edges(Coordinate(2.9, -3),
              {
        Coordinate(3, -3),
        Coordinate(-1, -1),
        Coordinate(0, -1),
        Coordinate(2, -2),
        Coordinate(0.3, -0.5),
              }, expectedVisGraph);
    add_edges(Coordinate(4, -2),
              {
        Coordinate(3, -3),
        Coordinate(3, -1),
              }, expectedVisGraph);
    add_edges(Coordinate(1, 0),
              {
        Coordinate(3, -1),
        Coordinate(2, -2),
        Coordinate(0, -1),
        Coordinate(0, 1),
        Coordinate(0.3, -0.5),
              }, expectedVisGraph);

    REQUIRE(visgraph == expectedVisGraph);
}


TEST_CASE("Visgraph Generator Surrounded Case") {
    const auto polygon = Polygon({
        Coordinate(1.5, 1),
        Coordinate(-1.5, 1),
        Coordinate(-1.5, -2),
        Coordinate(-1, -1),
        Coordinate(0, 0),
        Coordinate(1, -1),
        Coordinate(1.5, -2),
    });

    const auto visgraph = VisgraphGenerator::generate(std::vector<Polygon> {polygon});

    auto expectedVisGraph = Graph({polygon});
    add_edges(Coordinate(1.5, -2), {
        Coordinate(-1, -1),
        Coordinate(0, 0),
        Coordinate(1, -1),
        Coordinate(-1.5, -2),
        Coordinate(1.5, 1),
    }, expectedVisGraph);
    add_edges(Coordinate(-1.5, -2), {
        Coordinate(-1.5, 1),
        Coordinate(-1, -1),
        Coordinate(0, 0),
        Coordinate(1.5, -2),
        Coordinate(1, -1),
    }, expectedVisGraph);
    add_edges(Coordinate(0, 0), {
        Coordinate(1.5, -2),
        Coordinate(1, -1),
        Coordinate(-1, -1),
        Coordinate(-1.5, -2),
    }, expectedVisGraph);
    add_edges(Coordinate(-1, -1), {
        Coordinate(1.5, -2),
        Coordinate(-1.5, -2),
        Coordinate(1, -1),
        Coordinate(0, 0),
    }, expectedVisGraph);
    add_edges(Coordinate(1, -1), {
        Coordinate(1.5, -2),
        Coordinate(0, 0),
        Coordinate(-1, -1),
        Coordinate(-1.5, -2),
    }, expectedVisGraph);
    add_edges(Coordinate(-1.5, 1), {
        Coordinate(-1.5, -2),
        Coordinate(1.5, 1),
    }, expectedVisGraph);
    add_edges(Coordinate(1.5, 1), {
        Coordinate(1.5, -2),
        Coordinate(-1.5, 1),
    }, expectedVisGraph);
}

void add_edges(const Coordinate& source, const std::vector<Coordinate>& neighbors, Graph& g) {
    for (const auto& neighbor: neighbors) {
        g.add_edge(source, neighbor);
    }
}
