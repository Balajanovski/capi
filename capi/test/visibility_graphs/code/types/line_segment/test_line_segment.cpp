//
// Created by James.Balajan on 31/03/2021.
//

#include <catch.hpp>

#include "types/coordinate/coordinate.hpp"
#include "types/line_segment/line_segment.hpp"

TEST_CASE("Line Segment Constructor") {
    const auto endpoint_1 = Coordinate(1.0, 0.0);
    const auto endpoint_2 = Coordinate(0.0, 1.0);
    const auto line_segment = LineSegment(endpoint_1, endpoint_2);

    REQUIRE(line_segment.get_endpoint_1() == endpoint_1);
    REQUIRE(line_segment.get_endpoint_2() == endpoint_2);
}

TEST_CASE("Intersection with ray") {
    const auto endpoint_1 = Coordinate(1.0, 2.0);
    const auto endpoint_2 = Coordinate(2.0, 1.0);
    const auto line_segment = LineSegment(endpoint_1, endpoint_2);

    REQUIRE(Coordinate(1.5, 1.5) == *line_segment.intersection_with_ray(Coordinate(1, 1), Coordinate(1, 1)));
    REQUIRE_FALSE(line_segment.intersection_with_ray(Coordinate(1, 1), Coordinate(-1, -1)).has_value());
    REQUIRE_FALSE(line_segment.intersection_with_ray(Coordinate(1, 1), Coordinate(-1, 1)).has_value());
}
