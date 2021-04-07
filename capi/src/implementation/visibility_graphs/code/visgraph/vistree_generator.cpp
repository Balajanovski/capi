//
// Created by James.Balajan on 6/04/2021.
//

#include <algorithm>
#include <fmt/format.h>

#include "vistree_generator.hpp"
#include "geom/angle_sorter/angle_sorter.hpp"
#include "geom/line_segment_orientation_to_ray/line_segment_orientation_to_ray.hpp"
#include "constants/constants.hpp"


std::vector<Coordinate> VistreeGenerator::get_visible_vertices_from_root(const Coordinate &observer, const std::vector<Polygon>& polygons) {
    const auto all_polygon_vertices_and_incident_segments = VistreeGenerator::all_vertices_and_incident_segments(
            polygons);
    if (all_polygon_vertices_and_incident_segments.empty()) {
        return {};
    }

    auto vertices_sorted_clockwise_around_observer = VistreeGenerator::all_vertices(
            all_polygon_vertices_and_incident_segments);
    const auto nudged_observer = VistreeGenerator::nudge_observer_out_of_polygon(observer, all_polygon_vertices_and_incident_segments);
    AngleSorter::sort_counter_clockwise_around_observer(nudged_observer, vertices_sorted_clockwise_around_observer);

    auto open_edges = VistreeGenerator::OpenEdges();
    const auto initial_scanline_vector = Coordinate(MAX_LONGITUDE, nudged_observer.get_latitude());
    auto line_segments = VistreeGenerator::all_line_segments(all_polygon_vertices_and_incident_segments);
    for (const auto& line_segment: line_segments) {
        if (nudged_observer == line_segment.get_endpoint_1() || nudged_observer == line_segment.get_endpoint_2()) {
            continue;
        }

        const auto intersection = line_segment.intersection_with_segment(LineSegment(nudged_observer, initial_scanline_vector));
        if (intersection.has_value() && !(line_segment.get_endpoint_1() - nudged_observer).parallel(initial_scanline_vector) && !(line_segment.get_endpoint_2() - nudged_observer).parallel(initial_scanline_vector)) {
            open_edges.emplace((intersection.value() - nudged_observer).magnitude_squared(), std::make_unique<LineSegment>(line_segment));
        }
    }

    std::optional<Coordinate> prev_vertex = {};
    std::optional<bool> prev_vertex_visible = {};
    std::vector<Coordinate> visible_vertices;
    for (const auto& current_vertex: vertices_sorted_clockwise_around_observer) {
        const auto scanline_vector = current_vertex - nudged_observer;
        const auto& incident_segments = all_polygon_vertices_and_incident_segments.at(current_vertex);
        const auto clockwise_segments = VistreeGenerator::clockwise_segments(incident_segments, nudged_observer, scanline_vector);
        const auto counter_clockwise_segments = VistreeGenerator::counter_clockwise_segments(incident_segments, nudged_observer, scanline_vector);
        const auto collinear_segments = VistreeGenerator::collinear_segments(incident_segments, nudged_observer, scanline_vector);

        VistreeGenerator::erase_segments_from_open_edges(clockwise_segments, open_edges);

        const auto curr_vertex_visible = VistreeGenerator::is_vertex_visible(open_edges, nudged_observer, current_vertex, prev_vertex, prev_vertex_visible);
        if (curr_vertex_visible) {
            visible_vertices.push_back(current_vertex);
        }

        VistreeGenerator::add_segments_to_open_edges(counter_clockwise_segments, open_edges, nudged_observer, current_vertex);

        prev_vertex = current_vertex;
        prev_vertex_visible = curr_vertex_visible;
    }

    return visible_vertices;
}

VistreeGenerator::VertexToSegmentMapping VistreeGenerator::all_vertices_and_incident_segments(const std::vector<Polygon>& polygons) {
    auto vertices_and_segments = VistreeGenerator::VertexToSegmentMapping();
    for (const auto& polygon : polygons) {
        const auto& polygon_vertices = polygon.get_vertices();
        const auto num_poly_vertices = static_cast<long>(polygon_vertices.size());

        vertices_and_segments.reserve(polygon_vertices.size());

        for (long i = 0; i < num_poly_vertices; ++i) {
            long prev_idx = (i-1) % num_poly_vertices;
            if (prev_idx < 0) prev_idx += num_poly_vertices;
            long next_idx = (i+1) % num_poly_vertices;

            const auto& curr_vertex = polygon_vertices[i];
            const auto& prev_vertex = polygon_vertices[prev_idx];
            const auto& next_vertex = polygon_vertices[next_idx];

            const auto incident_segments = std::vector<LineSegment> {
                LineSegment(prev_vertex, curr_vertex),
                LineSegment(curr_vertex, next_vertex),
            };
            vertices_and_segments[curr_vertex] = incident_segments;
        }
    }

    return vertices_and_segments;
}

std::vector<LineSegment>
VistreeGenerator::all_line_segments(const VertexToSegmentMapping &vertices_and_segments) {
    std::vector<LineSegment> segments;

    for (const auto& vertex_and_segments : vertices_and_segments) {
        segments.reserve(vertex_and_segments.second.size());
        segments.insert(segments.end(), vertex_and_segments.second.begin(), vertex_and_segments.second.end());
    }

    return segments;
}

std::vector<Coordinate>
VistreeGenerator::all_vertices(const VertexToSegmentMapping &vertices_and_segments) {
    std::vector<Coordinate> vertices;
    vertices.reserve(vertices_and_segments.size());

    for (const auto& vertex_and_segments : vertices_and_segments) {
        vertices.push_back(vertex_and_segments.first);
    }

    return vertices;
}

std::vector<LineSegment>
VistreeGenerator::clockwise_segments(const std::vector<LineSegment> &segments, const Coordinate &ray_base,
                                     const Coordinate &ray_direction) {
    auto clockwise_segments = std::vector<LineSegment>();

    for (const auto& segment: segments) {
        const auto segment_orientation = line_segment_orientation_to_ray(segment, ray_base, ray_direction);
        if (segment_orientation == Orientation::CLOCKWISE) {
            clockwise_segments.push_back(segment);
        }
    }

    return clockwise_segments;
}

std::vector<LineSegment>
VistreeGenerator::counter_clockwise_segments(const std::vector<LineSegment> &segments, const Coordinate &ray_base,
                                             const Coordinate &ray_direction) {
    auto counter_clockwise_segments = std::vector<LineSegment>();

    for (const auto& segment: segments) {
        const auto segment_orientation = line_segment_orientation_to_ray(segment, ray_base, ray_direction);
        if (ray_base != segment.get_endpoint_1() && ray_base != segment.get_endpoint_2() && (segment_orientation == Orientation::COUNTER_CLOCKWISE)) {
            counter_clockwise_segments.push_back(segment);
        }
    }

    return counter_clockwise_segments;
}

std::vector<LineSegment>
VistreeGenerator::collinear_segments(const std::vector<LineSegment> &segments, const Coordinate &ray_base,
                                     const Coordinate &ray_direction) {
    auto collinear_segments = std::vector<LineSegment>();

    for (const auto& segment: segments) {
        const auto segment_orientation = line_segment_orientation_to_ray(segment, ray_base, ray_direction);
        if ((segment_orientation == Orientation::COLLINEAR)) {
            collinear_segments.push_back(segment);
        }
    }

    return collinear_segments;
}

bool VistreeGenerator::is_vertex_visible(const VistreeGenerator::OpenEdges &open_edges,
                                         const Coordinate &observer_coordinate,
                                         const Coordinate &vertex_in_question,
                                         const std::optional<Coordinate>& prev_vertex,
                                         const std::optional<bool>& prev_vertex_visible) {
    if (!open_edges.empty()) {
        const auto& closest_edge = open_edges.begin()->second;
        const auto intersection = closest_edge->intersection_with_segment(LineSegment(observer_coordinate, vertex_in_question));
        if (intersection.has_value()) {
            return false;
        }
    }

    return true;
}

void VistreeGenerator::erase_segments_from_open_edges(const std::vector<LineSegment> &segments,
                                                      VistreeGenerator::OpenEdges &open_edges) {
    for (const auto& segment : segments) {
        for (auto iter = open_edges.begin(); iter != open_edges.end(); ++iter) {
            if (*(iter->second) == segment) {
                open_edges.erase(iter);
                break;
            }
        }
    }
}

void VistreeGenerator::add_segments_to_open_edges(const std::vector<LineSegment> &segments,
                                                  VistreeGenerator::OpenEdges &open_edges,
                                                  const Coordinate& observer,
                                                  const Coordinate& current_vertex) {
    for (const auto& segment : segments) {
        const auto intersection = segment.intersection_with_segment(LineSegment(observer, current_vertex));
        const auto distance_squared = (intersection.value() - observer).magnitude_squared();

        open_edges.emplace(distance_squared, std::make_unique<LineSegment>(segment));
    }
}

Coordinate VistreeGenerator::nudge_observer_out_of_polygon(const Coordinate &observer,
                                                           const VistreeGenerator::VertexToSegmentMapping &vertices_and_segments) {
    if (vertices_and_segments.find(observer) == vertices_and_segments.end()) {
        return observer;
    }

    auto nudge_vec = Coordinate(0, 0);
    for (const auto& incident_segment: vertices_and_segments.at(observer)) {
        nudge_vec = nudge_vec + incident_segment.get_normal_vector();
    }
    nudge_vec = nudge_vec.unit_vector();

    return observer + (nudge_vec * (EPSILON_TOLERANCE * 10));
}