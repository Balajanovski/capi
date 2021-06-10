//
// Created by James.Balajan on 8/06/2021.
//

#ifndef CAPI_SPATIAL_INDEX_HPP
#define CAPI_SPATIAL_INDEX_HPP

#include <s2/mutable_s2shape_index.h>
#include <s2/s2polyline.h>
#include <vector>

#include "types/polygon/polygon.hpp"
#include "types/coordinate/coordinate.hpp"
#include "types/line_segment/line_segment.hpp"

class SpatialSegmentIndex {
  public:
    explicit SpatialSegmentIndex(const std::vector<Polygon> &polygons);
    ~SpatialSegmentIndex();

    [[nodiscard]] std::vector<LineSegment> segments_within_distance_of_point(const Coordinate &point,
                                                                             double distance_in_radians) const;
    [[nodiscard]] bool does_segment_intersect_with_segments(const LineSegment &segment) const;

  private:
    MutableS2ShapeIndex _index;
    std::vector<S2Polyline *> _polylines;
};

#endif // CAPI_SPATIAL_INDEX_HPP
