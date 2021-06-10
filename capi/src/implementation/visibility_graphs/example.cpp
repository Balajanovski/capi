//
// Created by James.Balajan on 8/06/2021.
//

#include <iostream>

#include "types/polygon/polygon.hpp"
#include "visgraph/vistree_generator.hpp"

int main() {
    const auto poly1 = Polygon({
        Coordinate(1., 0.),
        Coordinate(1., 10.),
        Coordinate(2., 10.),
        Coordinate(2., -1.),
        Coordinate(-2., -1.),
        Coordinate(-2., 10.),
        Coordinate(-1., 10.),
        Coordinate(-1., 0.),
    });

    const auto poly2 = Polygon({
        Coordinate(-357., -2.),
        Coordinate(-356., -3.),
        Coordinate(-357., -4.),
        Coordinate(-358., -3.),
    });

    const auto root = Coordinate(0., 1.);

    auto visible_vertices = VistreeGenerator(std::vector<Polygon>{poly1, poly2}).get_visible_vertices(root);
    for (const auto &visible_vertex : visible_vertices) {
        std::cout << visible_vertex << std::endl;
    }
}
