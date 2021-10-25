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

TEST_CASE("Line Segment Intersection with segment") {
    const auto endpoint_1 = Coordinate(1.0, 2.0);
    const auto endpoint_2 = Coordinate(2.0, 1.0);
    const auto line_segment = LineSegment(endpoint_1, endpoint_2);

    REQUIRE(Coordinate(1.5, 1.5) ==
            line_segment.intersection_with_segment(LineSegment(Coordinate(1., 1.), Coordinate(2., 2.))).value());
    REQUIRE_FALSE(
        line_segment.intersection_with_segment(LineSegment(Coordinate(1., 1.), Coordinate(1.1, 1.1))).has_value());
    REQUIRE(Coordinate(1.0, 2.0) ==
            line_segment.intersection_with_segment(LineSegment(Coordinate(1., 1.), Coordinate(1., 2.))).value());
    REQUIRE(Coordinate(2.0, 1.0) ==
            line_segment.intersection_with_segment(LineSegment(Coordinate(1., 1.), Coordinate(2., 1.))).value());
    REQUIRE_FALSE(
        line_segment.intersection_with_segment(LineSegment(Coordinate(1., 1.), Coordinate(0., 0.))).has_value());
    REQUIRE_FALSE(
        line_segment.intersection_with_segment(LineSegment(Coordinate(1., 1.), Coordinate(0., 2.))).has_value());
    REQUIRE_FALSE(
        line_segment.intersection_with_segment(LineSegment(Coordinate(1.5, 1.5), Coordinate(2.5, 0.5))).has_value());
    REQUIRE_FALSE(
        line_segment.intersection_with_segment(LineSegment(Coordinate(1.5, 1.5), Coordinate(0.5, 2.5))).has_value());
    REQUIRE(Coordinate(2., 1.) ==
            line_segment.intersection_with_segment(LineSegment(Coordinate(3., 0.), Coordinate(2., 1.))).value());
    REQUIRE(Coordinate(1., 2.) ==
            line_segment.intersection_with_segment(LineSegment(Coordinate(0., 3.), Coordinate(1., 2.))).value());
}

TEST_CASE("Line Segment Equals operator") {
    const auto seg1 = LineSegment(Coordinate(1.0, 2.0), Coordinate(2.0, 1.0));
    const auto seg2 = LineSegment(Coordinate(2.0, 1.0), Coordinate(1.0, 2.0));
    const auto seg3 = LineSegment(Coordinate(1.0, 1.0), Coordinate(2.0, 1.0));

    REQUIRE(seg1 == seg1);
    REQUIRE_FALSE(seg1 != seg1);
    REQUIRE(seg2 == seg1);
    REQUIRE_FALSE(seg2 != seg1);
    REQUIRE_FALSE(seg1 == seg3);
    REQUIRE(seg1 != seg3);
}

TEST_CASE("Line Segment Orientation of point to segment") {
    const auto segment = LineSegment(Coordinate(1., 0.), Coordinate(0., 1.));
    const auto p1 = Coordinate(0.3, 0.2);
    const auto p2 = Coordinate(0.5, 0.5);
    const auto p3 = Coordinate(0.7, 0.8);

    REQUIRE(segment.orientation_of_point_to_segment(p1) == Orientation::COUNTER_CLOCKWISE);
    REQUIRE(segment.orientation_of_point_to_segment(p2) == Orientation::COLLINEAR);
    REQUIRE(segment.orientation_of_point_to_segment(p3) == Orientation::CLOCKWISE);
}

TEST_CASE("Line Segment On Segment") {
    const auto segment = LineSegment(Coordinate(1., 0.), Coordinate(0., 1.));

    REQUIRE(segment.on_segment(Coordinate(1., 0.)));
    REQUIRE(segment.on_segment(Coordinate(0., 1.)));
    REQUIRE(segment.on_segment(Coordinate(0.5, 0.5)));
    REQUIRE_FALSE(segment.on_segment(Coordinate(2., -1.)));
    REQUIRE_FALSE(segment.on_segment(Coordinate(-1., 2.)));
    REQUIRE_FALSE(segment.on_segment(Coordinate(0., 0.)));
    REQUIRE_FALSE(segment.on_segment(Coordinate(1., 1.)));
}

TEST_CASE("Line Segment Project") {
    const auto segment_1 = LineSegment(Coordinate(0., 1.), Coordinate(1., 0.));
    const auto segment_2 = LineSegment(Coordinate(179., 0.), Coordinate(-179., 0.));

    const auto point_1 = Coordinate(1., 1.);
    const auto point_2 = Coordinate(179.5, 1.);

    const auto proj_point_1 = segment_1.project(point_1);
    const auto proj_point_2 = segment_2.project(point_2);

    const auto expected_proj_point_1 = Coordinate(0.5, 0.5);
    const auto expected_proj_point_2 = Coordinate(179.5, 0.);

    REQUIRE(std::abs(proj_point_1.get_longitude() - expected_proj_point_1.get_longitude()) < 0.0001);
    REQUIRE(std::abs(proj_point_1.get_latitude() - expected_proj_point_1.get_latitude()) < 0.0001);
    REQUIRE(std::abs(proj_point_2.get_longitude() - expected_proj_point_2.get_longitude()) < 0.0001);
    REQUIRE(std::abs(proj_point_2.get_latitude() - expected_proj_point_2.get_latitude()) < 0.0001);
}
