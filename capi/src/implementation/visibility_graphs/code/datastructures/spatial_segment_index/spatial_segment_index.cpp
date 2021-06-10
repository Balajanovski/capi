//
// Created by James.Balajan on 8/06/2021.
//

#include <memory>
#include <s2/s2closest_edge_query.h>
#include <s2/s2contains_point_query.h>
#include <s2/s2crossing_edge_query.h>

#include "spatial_segment_index.hpp"

SpatialSegmentIndex::SpatialSegmentIndex(const std::vector<Polygon> &polygons) {
    _loops.reserve(polygons.size());

    for (const auto &polygon : polygons) {
        const auto loop = polygon.to_s2_loop();
        _index.Add(std::make_unique<S2Loop::Shape>(loop));
        _loops.push_back(loop);
    }
}

SpatialSegmentIndex::~SpatialSegmentIndex() {
    for (const auto &loop : _loops) {
        delete loop;
    }
}

std::vector<LineSegment> SpatialSegmentIndex::segments_within_distance_of_point(const Coordinate &point,
                                                                                double distance_in_radians) const {
    S2ClosestEdgeQuery query(&_index);
    query.mutable_options()->set_max_distance(S1Angle::Radians(distance_in_radians));

    decltype(query)::PointTarget target(point.to_s2_point());
    const auto results = query.FindClosestEdges(&target);

    std::vector<LineSegment> segments;
    segments.reserve(results.size());
    for (const auto &result : results) {
        if (result.is_empty()) {
            continue;
        }

        const auto edge = query.GetEdge(result);
        segments.emplace_back(Coordinate(edge.v0), Coordinate(edge.v1));
    }

    return segments;
}

LineSegment SpatialSegmentIndex::closest_segment_to_point(const Coordinate &point) const {
    S2ClosestEdgeQuery query(&_index);
    query.mutable_options()->set_max_results(1);

    decltype(query)::PointTarget target(point.to_s2_point());
    const auto result = query.FindClosestEdge(&target);

    if (result.is_empty()) {
        throw std::runtime_error("No results found in closest_segment_to_point");
    }

    const auto edge = query.GetEdge(result);
    return LineSegment(Coordinate(edge.v0), Coordinate(edge.v1));
}

bool SpatialSegmentIndex::does_segment_intersect_with_segments(const LineSegment &segment) const {
    S2CrossingEdgeQuery query(&_index);

    const auto results =
        query.GetCrossingEdges(segment.get_endpoint_1().to_s2_point(), segment.get_endpoint_2().to_s2_point(),
                               S2CrossingEdgeQuery::CrossingType::ALL);
    return !results.empty();
}

bool SpatialSegmentIndex::is_point_contained(const Coordinate &point) const {
    S2ContainsPointQueryOptions options(S2VertexModel::OPEN);
    auto query = MakeS2ContainsPointQuery(&_index, options);
    return query.Contains(point.to_s2_point());
}