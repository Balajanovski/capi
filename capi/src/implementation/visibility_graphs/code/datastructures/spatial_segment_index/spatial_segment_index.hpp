//
// Created by James.Balajan on 8/06/2021.
//

#ifndef CAPI_SPATIAL_INDEX_HPP
#define CAPI_SPATIAL_INDEX_HPP

#include <s2/mutable_s2shape_index.h>
#include <s2/s2polyline.h>
#include <vector>

#include "types/coordinate/coordinate.hpp"
#include "types/line_segment/line_segment.hpp"
#include "datastructures/graph/graph.hpp"


class SpatialSegmentIndex {
public:
    explicit SpatialSegmentIndex(const Graph &graph);
    ~SpatialSegmentIndex();

    [[nodiscard]] std::vector<LineSegment> segments_within_distance_of_point(const Coordinate &point, double distance_in_radians) const;
private:
    MutableS2ShapeIndex _index;
    std::vector<S2Polyline*> _polylines;
};


#endif //CAPI_SPATIAL_INDEX_HPP
