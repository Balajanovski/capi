//
// Created by James.Balajan on 6/04/2021.
//

#include <algorithm>
#include <map>

#include "vistree_generator.hpp"
#include "geom/angle_sorter/angle_sorter.hpp"
#include "geom/line_segment_orientation_to_ray/line_segment_orientation_to_ray.hpp"


std::vector<Coordinate> VistreeGenerator::get_visible_vertices_from_root(const Coordinate &root_vertex, const std::vector<Polygon>& polygons) {
    const auto all_polygon_vertices_and_incident_segments = VistreeGenerator::all_vertices_and_incident_segments(
            polygons);
    if (all_polygon_vertices_and_incident_segments.empty()) {
        return {};
    }

    auto sorted_vertices_excluding_root = VistreeGenerator::all_vertices_excluding_root(all_polygon_vertices_and_incident_segments, root_vertex);
    AngleSorter::sort_counter_clockwise_around_root_vertex(root_vertex, sorted_vertices_excluding_root);

    const auto initial_scanline_vector = sorted_vertices_excluding_root.front() - root_vertex;
    auto open_edges = std::map<double, const LineSegment*>();
    auto line_segments = VistreeGenerator::all_line_segments(all_polygon_vertices_and_incident_segments);

    for (const auto& line_segment: line_segments) {
        const auto intersection = line_segment.intersection_with_ray(root_vertex, initial_scanline_vector);
        if (intersection.has_value()) {
            open_edges[(intersection.value() - root_vertex).magnitude_squared()] = &line_segment;
        }
    }

    std::vector<Coordinate> visible_vertices;
    for (const auto& vertex: sorted_vertices_excluding_root) {
        const auto& closest_edge = open_edges.begin()->second;
        if (open_edges.empty() || !closest_edge->intersection_with_ray(root_vertex, vertex - root_vertex).has_value()) {
            visible_vertices.push_back(vertex);
        }

        const auto scanline_vector = vertex - root_vertex;
        const auto& incident_segments = all_polygon_vertices_and_incident_segments.at(vertex);
        for (const auto& incident_segment: incident_segments) {
            const auto segment_orientation = line_segment_orientation_to_ray(incident_segment, root_vertex, scanline_vector);
            if (segment_orientation == Orientation::CLOCKWISE) {
                for (auto iter = open_edges.begin(); iter != open_edges.end(); ++iter) {
                    if (*(iter->second) == incident_segment) {
                        open_edges.erase(iter);
                        break;
                    }
                }
            } else if (segment_orientation == Orientation::COUNTER_CLOCKWISE) {
                auto intersection = incident_segment.intersection_with_ray(root_vertex, scanline_vector);
                open_edges[(intersection.value() - root_vertex).magnitude_squared()] = &incident_segment;
            }
        }
    }

    return visible_vertices;
}

std::unordered_map<Coordinate, std::vector<LineSegment>> VistreeGenerator::all_vertices_and_incident_segments(const std::vector<Polygon>& polygons) {
    auto vertices_and_segments = std::unordered_map<Coordinate, std::vector<LineSegment>>();
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
VistreeGenerator::all_line_segments(const std::unordered_map<Coordinate, std::vector<LineSegment>>& vertices_and_segments) {
    std::vector<LineSegment> segments;

    for (const auto& vertex_and_segments : vertices_and_segments) {
        segments.reserve(vertex_and_segments.second.size());
        segments.insert(segments.end(), vertex_and_segments.second.begin(), vertex_and_segments.second.end());
    }

    return segments;
}

std::vector<Coordinate>
VistreeGenerator::all_vertices_excluding_root(const std::unordered_map<Coordinate, std::vector<LineSegment>> &vertices_and_segments, const Coordinate& root) {
    std::vector<Coordinate> vertices;
    vertices.reserve(vertices_and_segments.size());

    for (const auto& vertex_and_segments : vertices_and_segments) {
        if (root != vertex_and_segments.first) {
            vertices.push_back(vertex_and_segments.first);
        }
    }

    return vertices;
}
