//
// Created by James.Balajan on 8/06/2021.
//

#ifndef CAPI_SPATIAL_INDEX_HPP
#define CAPI_SPATIAL_INDEX_HPP

#include <s2/mutable_s2shape_index.h>
#include <s2/s2loop.h>
#include <s2/s2shapeutil_shape_edge.h>
#include <vector>

#include "types/coordinate/coordinate.hpp"
#include "types/line_segment/line_segment.hpp"
#include "types/polygon/polygon.hpp"
#include "types/visible_vertex/visible_vertex.hpp"

using s2shapeutil::ShapeEdge;

class SpatialSegmentIndex {
  public:
    explicit SpatialSegmentIndex(const std::vector<Polygon> &polygons);
    ~SpatialSegmentIndex();

    [[nodiscard]] std::vector<LineSegment> segments_within_distance_of_point(const Coordinate &point,
                                                                             double distance_in_radians) const;
    [[nodiscard]] LineSegment closest_segment_to_point(const Coordinate &point) const;

    [[nodiscard]] std::vector<LineSegment> intersect_with_segments(const LineSegment &segment) const;
    [[nodiscard]] bool does_segment_intersect_with_segments(const LineSegment &segment) const;
    [[nodiscard]] bool is_point_contained(const Coordinate &point) const;

  private:
    MutableS2ShapeIndex _shape_index;

    std::vector<S2Loop *> _loops;
    static LineSegment s2_to_capi_line_segment(s2shapeutil::ShapeEdge edge);
};

#endif // CAPI_SPATIAL_INDEX_HPP
