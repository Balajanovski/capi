//
// Created by James.Balajan on 10/06/2021.
//

#include <catch.hpp>
#include <vector>

#include "types/coordinate/coordinate.hpp"
#include "types/line_segment/line_segment.hpp"
#include "types/polygon/polygon.hpp"
#include "datastructures/spatial_segment_index/spatial_segment_index.hpp"


TEST_CASE("Test segments within distance of point") {
    const auto polygons = std::vector<Polygon> {
        Polygon({
            Coordinate(1., 1.),
            Coordinate(2., 0.),
            Coordinate(1., -1.),
        })
    };
    const auto index = SpatialSegmentIndex(polygons);

    REQUIRE(index.segments_within_distance_of_point(Coordinate(0, 0),
                                                    0.01745329252 - 0.000001).empty());
    const auto segments = index.segments_within_distance_of_point(
            Coordinate(0, 0),
            0.01745329252 + 0.000001
    );
    REQUIRE(segments == std::vector<LineSegment> {
        LineSegment(Coordinate(1., 1.), Coordinate(1.,  -1.))
    });
}


TEST_CASE("Test does_segment_intersect_with_segments") {
    const auto polygons = std::vector<Polygon> {
        Polygon({
            Coordinate(1., 1.),
            Coordinate(1., -1.),
            Coordinate(-1., -1.),
            Coordinate(-1., 1.),
        })
    };
    const auto index = SpatialSegmentIndex(polygons);

    const auto seg1 = LineSegment(Coordinate(0., 0.), Coordinate(1., 0.));
    const auto seg2 = LineSegment(Coordinate(50., 0.), Coordinate(60., 0.));

    REQUIRE(index.does_segment_intersect_with_segments(seg1));
    REQUIRE_FALSE(index.does_segment_intersect_with_segments(seg2));
}

TEST_CASE("Test is point contained") {
    const auto polygons = std::vector<Polygon> {
            Polygon({
                            Coordinate(1., 1.),
                            Coordinate(1., -1.),
                            Coordinate(-1., -1.),
                            Coordinate(-1., 1.),
                    })
    };
    const auto index = SpatialSegmentIndex(polygons);

    const auto p1 = Coordinate(0., 0.);
    const auto p2 = Coordinate(1., 1.);
    const auto p3 = Coordinate(2., 0.);

    REQUIRE(index.is_point_contained(p1));
    REQUIRE_FALSE(index.is_point_contained(p2));
    REQUIRE_FALSE(index.is_point_contained(p3));
}
