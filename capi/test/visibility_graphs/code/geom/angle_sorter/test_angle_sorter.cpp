//
// Created by James.Balajan on 6/04/2021.
//


#include <catch.hpp>
#include <vector>

#include "types/coordinate/coordinate.hpp"
#include "geom/angle_sorter/angle_sorter.hpp"

TEST_CASE("Test sort counter clockwise around root vertex") {
    const auto root = Coordinate(1, 1);
    std::vector<Coordinate> vertices {
        root,
        Coordinate(1, 2),
        Coordinate(4, 0),
        Coordinate(2, 0),
        Coordinate(2, 2),
        Coordinate(0, 1),
        Coordinate(1, 0),
    };
    const std::vector<Coordinate> expected_sorted_vertices {
        root,
        Coordinate(0, 1),
        Coordinate(1, 0),
        Coordinate(2, 0),
        Coordinate(4, 0),
        Coordinate(2, 2),
        Coordinate(1, 2),
    };

    AngleSorter::sort_counter_clockwise_around_observer(root, vertices);

    REQUIRE(expected_sorted_vertices == vertices);
}
