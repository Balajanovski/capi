//
// Created by James.Balajan on 8/06/2021.
//

#include <memory>
#include <s2/s2closest_point_query.h>
#include <s2/s2closest_edge_query_testing.h>
#include <s2/s2closest_edge_query.h>
#include <s2/s2contains_point_query.h>
#include <s2/s2crossing_edge_query.h>
#include <s2/s2earth.h>
#include <fmt/core.h>
#include "spatial_segment_index.hpp"

SpatialSegmentIndex::SpatialSegmentIndex(const std::vector<Polygon> &polygons) {
    _loops.reserve(polygons.size());

    for (const auto &polygon : polygons) {
        for (const auto &point : polygon.get_vertices()) {
            _point_index.Add(point.to_s2_point(), 0);
        }
        const auto loop = polygon.to_s2_loop();
        _shape_index.Add(std::make_unique<S2Loop::Shape>(loop));
        _loops.push_back(loop);
    }

    _closest_point_query = std::make_unique<S2ClosestPointQuery<int>>(&_point_index);
    _contains_point_query = std::make_unique<S2ContainsPointQuery<MutableS2ShapeIndex>>(&_shape_index, S2ContainsPointQueryOptions(S2VertexModel::OPEN));
    _crossing_edge_query = std::make_unique<S2CrossingEdgeQuery>(&_shape_index);
    _closest_edge_query = std::make_unique<S2ClosestEdgeQuery>(&_shape_index);
}

SpatialSegmentIndex::~SpatialSegmentIndex() {
    for (const auto &loop : _loops) {
        delete loop;
    }
}

std::vector<LineSegment> SpatialSegmentIndex::segments_within_distance_of_point(const Coordinate &point,
                                                                                const double distance_in_radians) const {
    _closest_edge_query->mutable_options()->set_max_distance(S1Angle::Radians(distance_in_radians));

    S2ClosestEdgeQuery::PointTarget target(point.to_s2_point());
    const auto results = _closest_edge_query->FindClosestEdges(&target);

    std::vector<LineSegment> segments;
    segments.reserve(results.size());
    for (const auto &result : results) {
        if (result.is_empty()) {
            continue;
        }

        const auto edge = _closest_edge_query->GetEdge(result);
        segments.emplace_back(Coordinate(edge.v0), Coordinate(edge.v1));
    }

    return segments;
}

LineSegment SpatialSegmentIndex::closest_segment_to_point(const Coordinate &point) const {
    _closest_edge_query->mutable_options()->set_max_results(1);

    S2ClosestEdgeQuery::PointTarget target(point.to_s2_point());
    const auto result = _closest_edge_query->FindClosestEdge(&target);

    if (result.is_empty()) {
        throw std::runtime_error("No results found in closest_segment_to_point");
    }

    const auto edge = _closest_edge_query->GetEdge(result);
    return LineSegment(Coordinate(edge.v0), Coordinate(edge.v1));
}

std::vector<Coordinate> SpatialSegmentIndex::points_visible_within_distance(const Coordinate &point, const double distance_in_radians) const {
    _closest_edge_query->mutable_options()->set_max_distance(S1Angle::Radians(distance_in_radians));

    S2ClosestEdgeQuery::PointTarget target(point.to_s2_point());
    const auto closest_edges = _closest_edge_query->FindClosestEdges(&target);

    std::vector<Coordinate> visible_points;
    visible_points.reserve(2* closest_edges.size());

    const auto observer = point.to_s2_point();
    for (const auto &result : closest_edges) {
        if (result.is_empty()) {
            continue;
        }

        const auto edge = _closest_edge_query->GetEdge(result);

        const auto v0_crossed_edges = _crossing_edge_query->GetCrossingEdges(observer, edge.v0, S2CrossingEdgeQuery::CrossingType::INTERIOR);
        if (v0_crossed_edges.empty()) {
            visible_points.emplace_back(edge.v0);
        }

        const auto v1_crossed_edges = _crossing_edge_query->GetCrossingEdges(observer, edge.v1, S2CrossingEdgeQuery::CrossingType::INTERIOR);
        if (v1_crossed_edges.empty()) {
            visible_points.emplace_back(edge.v1);
        }

    }

    return visible_points;

}


std::optional<Coordinate> SpatialSegmentIndex::closest_point_to_point(const Coordinate &point, std::optional<double> max_distance_radians) const {
    if (max_distance_radians.has_value()) {
        _closest_point_query->mutable_options()->set_max_distance(S1Angle::Radians(max_distance_radians.value()));
    }
    S2ClosestPointQueryPointTarget target(point.to_s2_point());
    const auto result = _closest_point_query->FindClosestPoint(&target);

    if (result.is_empty()) {
        return std::nullopt;
    }

    return Coordinate(result.point());
}

bool SpatialSegmentIndex::does_segment_intersect_with_segments(const LineSegment &segment) const {
    const auto results =
        _crossing_edge_query->GetCrossingEdges(
            segment.get_endpoint_1().to_s2_point(),
            segment.get_endpoint_2().to_s2_point(),
       S2CrossingEdgeQuery::CrossingType::ALL);

    return !results.empty();
}

std::vector<LineSegment> SpatialSegmentIndex::intersect_with_segments(const LineSegment &segment) const {
    auto p1 = segment.get_endpoint_1().to_s2_point();
    const auto p2 = segment.get_endpoint_2().to_s2_point();
    std::vector<LineSegment> result;
    auto edges = _crossing_edge_query->GetCrossingEdges(p1, p2,S2CrossingEdgeQuery::CrossingType::ALL);

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
    return _contains_point_query->Contains(point.to_s2_point());
}
