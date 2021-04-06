//
// Created by James.Balajan on 6/04/2021.
//

#include <catch.hpp>

#include "types/orientation/orientation.hpp"
#include "types/coordinate/coordinate.hpp"
#include "types/line_segment/line_segment.hpp"
#include "geom/line_segment_orientation_to_ray/line_segment_orientation_to_ray.hpp"

TEST_CASE("Line Segment Orientation to Ray") {
    const auto ray_base = Coordinate(1, 1);
    const auto ray_direction = Coordinate(1, 1);

    const auto point_counter_clockwise_of_ray_1 = Coordinate(1, 2);
    const auto point_counter_clockwise_of_ray_2 = Coordinate(2, 3);
    const auto point_clockwise_of_ray_1 = Coordinate(2, 1);
    const auto point_clockwise_of_ray_2 = Coordinate(3, 2);
    const auto point_collinear_to_ray_1 = Coordinate(2, 2);
    const auto point_collinear_to_ray_2 = Coordinate(3, 3);

    REQUIRE(line_segment_orientation_to_ray(LineSegment(point_clockwise_of_ray_1, point_clockwise_of_ray_2), ray_base, ray_direction) == Orientation::CLOCKWISE);
    REQUIRE(line_segment_orientation_to_ray(LineSegment(point_collinear_to_ray_1, point_collinear_to_ray_1), ray_base, ray_direction) == Orientation::CLOCKWISE);
    REQUIRE(line_segment_orientation_to_ray(LineSegment(point_counter_clockwise_of_ray_1, point_counter_clockwise_of_ray_2), ray_base, ray_direction) == Orientation::COUNTER_CLOCKWISE);
    REQUIRE(line_segment_orientation_to_ray(LineSegment(point_collinear_to_ray_2, point_counter_clockwise_of_ray_2), ray_base, ray_direction) == Orientation::COUNTER_CLOCKWISE);
    REQUIRE(line_segment_orientation_to_ray(LineSegment(point_collinear_to_ray_1, point_collinear_to_ray_2), ray_base, ray_direction));
}
