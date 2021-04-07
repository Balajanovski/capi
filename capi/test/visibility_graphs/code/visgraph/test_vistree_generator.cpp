//
// Created by James.Balajan on 7/04/2021.
//

#include <catch.hpp>
#include <vector>
#include <iostream>

#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"
#include "visgraph/vistree_generator.hpp"

TEST_CASE("Vistree Generator get visible vertices from root") {
    const auto root_vertex = Coordinate(1.000, 0.000);

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
        Coordinate(2.5, -3),
        Coordinate(3, -3),
        Coordinate(4, -2),
    });

    const auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root_vertex, std::vector<Polygon> {poly1, poly2});
    std::cout << visible_vertices.size() << "\n";
    for (const auto& v : visible_vertices) {
        std::cout << v << "\n";
    }
    std::cout << std::flush;

}
