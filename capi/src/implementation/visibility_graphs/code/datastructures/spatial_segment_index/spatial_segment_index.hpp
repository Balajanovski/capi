//
// Created by James.Balajan on 8/06/2021.
//

#ifndef CAPI_SPATIAL_INDEX_HPP
#define CAPI_SPATIAL_INDEX_HPP

#include <s2/mutable_s2shape_index.h>
#include <s2/s2loop.h>
#include <s2/s2contains_point_query.h>
#include <s2/s2closest_point_query.h>
#include <s2/s2closest_edge_query.h>
#include <s2/s2crossing_edge_query.h>
#include <s2/s2shapeutil_shape_edge.h>
#include <vector>

#include "types/coordinate/coordinate.hpp"
#include "types/line_segment/line_segment.hpp"
#include "types/polygon/polygon.hpp"

using s2shapeutil::ShapeEdge;

class SpatialSegmentIndex {
  public:
    explicit SpatialSegmentIndex(const std::vector<Polygon> &polygons);
    ~SpatialSegmentIndex();

    [[nodiscard]] std::vector<LineSegment> segments_within_distance_of_point(const Coordinate &point,
                                                                             double distance_in_radians) const;
    [[nodiscard]] std::vector<Coordinate> reachable_vertices(const Coordinate &point,
                                                             double max_edge_distance_in_radians) const;
    [[nodiscard]] LineSegment closest_segment_to_point(const Coordinate &point) const;
    [[nodiscard]] std::optional<Coordinate> closest_point_to_point(const Coordinate &point, std::optional<double> max_distance_radians = std::nullopt) const;

    [[nodiscard]] std::vector<LineSegment> intersect_with_segments(const LineSegment &segment) const;
    [[nodiscard]] bool does_segment_intersect_with_segments(const LineSegment &segment) const;
    [[nodiscard]] bool is_point_contained(const Coordinate &point) const;

  private:
    MutableS2ShapeIndex _shape_index;
    S2PointIndex<int> _point_index;
    std::unique_ptr<S2CrossingEdgeQuery> _crossing_edge_query;
    std::unique_ptr<S2ContainsPointQuery<MutableS2ShapeIndex>> _contains_point_query;
    std::unique_ptr<S2ClosestEdgeQuery> _closest_edge_query;
    std::unique_ptr<S2ClosestPointQuery<int>> _closest_point_query;
    std::vector<S2Loop *> _loops;
    static LineSegment s2_to_capi_line_segment(s2shapeutil::ShapeEdge edge);
};

#endif // CAPI_SPATIAL_INDEX_HPP
