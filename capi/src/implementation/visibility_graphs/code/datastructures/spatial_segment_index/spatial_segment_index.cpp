//
// Created by James.Balajan on 8/06/2021.
//

#include "spatial_segment_index.hpp"
#include <memory>
#include <s2/s2closest_edge_query.h>
#include <s2/s2contains_point_query.h>
#include <s2/s2crossing_edge_query.h>
#include <unordered_set>

SpatialSegmentIndex::SpatialSegmentIndex(const std::vector<Polygon> &polygons) {
    _loops.reserve(polygons.size());

    for (const auto &polygon : polygons) {
        const auto loop = polygon.to_s2_loop();
        _shape_index.Add(std::make_unique<S2Loop::Shape>(loop));
        _loops.push_back(loop);
    }
}

SpatialSegmentIndex::~SpatialSegmentIndex() {
    for (const auto &loop : _loops) {
        delete loop;
    }
}

std::vector<Coordinate> SpatialSegmentIndex::reachable_vertices(const Coordinate &point,
                                                                double distance_in_radians) const {
    const auto observer = point.to_s2_point();

    S2ClosestEdgeQuery closest_edge_query(&_shape_index);
    closest_edge_query.mutable_options()->set_max_distance(S1Angle::Radians(distance_in_radians));
    closest_edge_query.mutable_options()->set_include_interiors(false);

    S2CrossingEdgeQuery crossing_edge_query(&_shape_index);

    decltype(closest_edge_query)::PointTarget target(observer);
    const auto closest_edges = closest_edge_query.FindClosestEdges(&target);

    std::unordered_set<Coordinate> visible_points;
    visible_points.reserve(2 * closest_edges.size());
    for (const auto &result : closest_edges) {
        if (result.is_empty()) {
            continue;
        }

        const auto edge = closest_edge_query.GetEdge(result);

        const auto v0_crossed_edges =
            crossing_edge_query.GetCrossingEdges(observer, edge.v0, S2CrossingEdgeQuery::CrossingType::INTERIOR);
        if (v0_crossed_edges.empty()) {
            visible_points.emplace(edge.v0);
        }

        const auto v1_crossed_edges =
            crossing_edge_query.GetCrossingEdges(observer, edge.v1, S2CrossingEdgeQuery::CrossingType::INTERIOR);
        if (v1_crossed_edges.empty()) {
            visible_points.emplace(edge.v1);
        }
    }

    return std::vector<Coordinate>(visible_points.begin(), visible_points.end());
}

std::vector<LineSegment> SpatialSegmentIndex::segments_within_distance_of_point(const Coordinate &point,
                                                                                double distance_in_radians) const {
    S2ClosestEdgeQuery query(&_shape_index);
    query.mutable_options()->set_max_distance(S1Angle::Radians(distance_in_radians));
    query.mutable_options()->set_include_interiors(false);

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
    S2ClosestEdgeQuery query(&_shape_index);
    query.mutable_options()->set_max_results(1);
    query.mutable_options()->set_include_interiors(false);

    decltype(query)::PointTarget target(point.to_s2_point());
    const auto result = query.FindClosestEdge(&target);

    if (result.is_empty()) {
        throw std::runtime_error("No results found in closest_segment_to_point");
    }

    const auto edge = query.GetEdge(result);
    return LineSegment(Coordinate(edge.v0), Coordinate(edge.v1));
}

bool SpatialSegmentIndex::does_segment_intersect_with_segments(const LineSegment &segment) const {
    S2CrossingEdgeQuery query(&_shape_index);

    const auto results =
        query.GetCrossingEdges(segment.get_endpoint_1().to_s2_point(), segment.get_endpoint_2().to_s2_point(),
                               S2CrossingEdgeQuery::CrossingType::ALL);
    return !results.empty();
}

std::vector<LineSegment> SpatialSegmentIndex::intersect_with_segments(const LineSegment &segment) const {
    S2CrossingEdgeQuery query(&_shape_index);
    auto p1 = segment.get_endpoint_1().to_s2_point();
    const auto p2 = segment.get_endpoint_2().to_s2_point();
    std::vector<LineSegment> result;
    auto edges = query.GetCrossingEdges(p1, p2, S2CrossingEdgeQuery::CrossingType::ALL);

    std::sort(edges.begin(), edges.end(), [&p1](s2shapeutil::ShapeEdge &e1, s2shapeutil::ShapeEdge &e2) -> bool {
        return S2::GetDistance(p1, e1.v0(), e1.v1()) < S2::GetDistance(p1, e2.v0(), e2.v1());
    });

    std::transform(edges.begin(), edges.end(), std::back_inserter(result), s2_to_capi_line_segment);

    return result;
}

LineSegment SpatialSegmentIndex::s2_to_capi_line_segment(const s2shapeutil::ShapeEdge edge) {
    return LineSegment(Coordinate(edge.v0()), Coordinate(edge.v1()));
}

bool SpatialSegmentIndex::is_point_contained(const Coordinate &point) const {
    S2ContainsPointQueryOptions options(S2VertexModel::OPEN);
    auto query = MakeS2ContainsPointQuery(&_shape_index, options);
    return query.Contains(point.to_s2_point());
}
