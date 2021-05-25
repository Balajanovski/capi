//
// Created by James.Balajan on 6/04/2021.
//

#include <optional>
#include <stdexcept>
#include <unordered_set>

#include "constants/constants.hpp"
#include "coordinate_periodicity/coordinate_periodicity.hpp"
#include "geom/angle_sorter/angle_sorter.hpp"
#include "types/polyline/three_vertex_polyline.hpp"
#include "vistree_generator.hpp"

std::vector<VisibleVertex> VistreeGenerator::get_visible_vertices_from_root(const Coordinate &observer,
                                                                            const std::vector<Polygon> &polygons,
                                                                            bool half_scan) {
    const auto all_polygon_vertices_and_incident_segments =
        VistreeGenerator::all_vertices_and_incident_segments(polygons);
    if (all_polygon_vertices_and_incident_segments.empty()) {
        return {};
    }

    auto vertices_sorted_counter_clockwise_around_observer =
        VistreeGenerator::all_vertices(all_polygon_vertices_and_incident_segments);
    AngleSorter::sort_counter_clockwise_around_observer(observer, vertices_sorted_counter_clockwise_around_observer);

    auto open_edges = VistreeGenerator::OpenEdges();
    const auto initial_scanline_segment =
        LineSegment(observer, Coordinate(MAX_PERIODIC_LONGITUDE, observer.get_latitude()));
    const auto initial_scanline_vector = initial_scanline_segment.get_tangent_vector();
    auto line_segments = VistreeGenerator::all_line_segments(all_polygon_vertices_and_incident_segments);
    for (const auto &line_segment : line_segments) {
        if (observer == line_segment.get_endpoint_1() || observer == line_segment.get_endpoint_2()) {
            continue;
        }

        const auto intersection = line_segment.intersection_with_segment(initial_scanline_segment);
        if (intersection.has_value() && !initial_scanline_segment.on_segment(line_segment.get_endpoint_1()) &&
            !initial_scanline_segment.on_segment(line_segment.get_endpoint_2())) {
            open_edges.emplace((intersection.value() - observer).magnitude_squared(),
                               std::make_unique<LineSegment>(line_segment));
        }
    }

    bool prev_visible = false;
    auto prev_vertex = std::optional<Coordinate>{};
    std::vector<VisibleVertex> visible_vertices;
    for (const auto &current_vertex : vertices_sorted_counter_clockwise_around_observer) {
        if (current_vertex == observer) {
            continue;
        }

        const auto scanline_segment = LineSegment(observer, current_vertex);
        if (half_scan && initial_scanline_vector.cross_product_magnitude(scanline_segment.get_tangent_vector()) < 0) {
            break;
        }

        const auto &incident_segments = all_polygon_vertices_and_incident_segments.at(current_vertex);
        const auto clockwise_segments =
            VistreeGenerator::orientation_segments(incident_segments, scanline_segment, Orientation::CLOCKWISE);
        const auto counter_clockwise_segments =
            VistreeGenerator::orientation_segments(incident_segments, scanline_segment, Orientation::COUNTER_CLOCKWISE);

        VistreeGenerator::erase_segments_from_open_edges(clockwise_segments, open_edges);

        const auto curr_vertex_visible =
            VistreeGenerator::is_vertex_visible(open_edges, all_polygon_vertices_and_incident_segments, observer,
                                                current_vertex, prev_vertex, prev_visible);
        if (curr_vertex_visible) {
            visible_vertices.push_back(VisibleVertex{
                .coord = coordinate_from_periodic_coordinate(current_vertex),
                .is_visible_across_meridian = is_coordinate_over_meridian(current_vertex),
            });
        }

        VistreeGenerator::add_segments_to_open_edges(counter_clockwise_segments, open_edges, observer, current_vertex);

        prev_vertex = current_vertex;
        prev_visible = curr_vertex_visible;
    }

    return visible_vertices;
}

VistreeGenerator::VertexToSegmentMapping
VistreeGenerator::all_vertices_and_incident_segments(const std::vector<Polygon> &polygons) {
    auto vertices_and_segments = VistreeGenerator::VertexToSegmentMapping();
    for (const auto &polygon : polygons) {
        const auto &polygon_vertices = polygon.get_vertices();
        const auto num_poly_vertices = static_cast<long>(polygon_vertices.size());

        vertices_and_segments.reserve(polygon_vertices.size());

        for (long i = 0; i < num_poly_vertices; ++i) {
            long prev_idx = (i - 1) % num_poly_vertices;
            if (prev_idx < 0)
                prev_idx += num_poly_vertices;
            long next_idx = (i + 1) % num_poly_vertices;

            const auto &curr_vertex = polygon_vertices[i];
            const auto &prev_vertex = polygon_vertices[prev_idx];
            const auto &next_vertex = polygon_vertices[next_idx];

            const auto incident_segments = std::vector<LineSegment>{
                LineSegment(prev_vertex, curr_vertex),
                LineSegment(curr_vertex, next_vertex),
            };
            vertices_and_segments[curr_vertex] = incident_segments;
        }
    }

    return vertices_and_segments;
}

std::vector<LineSegment> VistreeGenerator::all_line_segments(const VertexToSegmentMapping &vertices_and_segments) {
    std::vector<LineSegment> segments;
    segments.reserve(vertices_and_segments.size());

    for (const auto &vertex_and_segments : vertices_and_segments) {
        if (vertex_and_segments.second.size() != 2) {
            throw std::runtime_error("Vertex detected which does not have only two segments");
        }

        segments.push_back(vertex_and_segments.second[0]);
    }

    return segments;
}

std::vector<Coordinate> VistreeGenerator::all_vertices(const VertexToSegmentMapping &vertices_and_segments) {
    std::vector<Coordinate> vertices;
    vertices.reserve(vertices_and_segments.size());

    for (const auto &vertex_and_segments : vertices_and_segments) {
        vertices.push_back(vertex_and_segments.first);
    }

    return vertices;
}

std::vector<LineSegment> VistreeGenerator::orientation_segments(const std::vector<LineSegment> &segments,
                                                                const LineSegment &scanline_segment,
                                                                const Orientation &desired_orientation) {
    auto orientation_segs = std::vector<LineSegment>();

    for (const auto &segment : segments) {
        const auto segment_orientation = scanline_segment.orientation_of_point_to_segment(
            segment.get_adjacent_to(scanline_segment.get_endpoint_2()));
        if (segment_orientation == desired_orientation) {
            orientation_segs.push_back(segment);
        }
    }

    return orientation_segs;
}

bool VistreeGenerator::is_vertex_visible_collinear(const VistreeGenerator::OpenEdges &open_edges, bool prev_visible,
                                                   const std::optional<Coordinate> &prev_vertex,
                                                   const Coordinate &current_vertex) {
    if (!prev_visible) {
        return false;
    }

    const auto prev_to_current_segment = LineSegment(prev_vertex.value(), current_vertex);
    for (const auto &open_edge_and_distance : open_edges) {
        const auto &open_edge = open_edge_and_distance.second;
        if (open_edge->intersection_with_segment(prev_to_current_segment).has_value()) {
            return false;
        }
    }

    return true;
}

bool VistreeGenerator::is_vertex_visible_non_collinear(
    const VistreeGenerator::OpenEdges &open_edges,
    const VistreeGenerator::VertexToSegmentMapping &vertices_and_segments, const Coordinate &observer_coordinate,
    const Coordinate &vertex_in_question) {
    if (vertex_in_question == observer_coordinate) {
        return false;
    }
    if (vertices_and_segments.find(observer_coordinate) != vertices_and_segments.end() &&
        vertices_and_segments.size() > 2) {
        // We perform this check to stop our observer coordinate (if it is a vertex of a polygon)
        // from seeing vertices from inside the polygon
        // Essentially we build artificial walls based on the edges we know obstruct vision (those adjacent)
        // This will be a constant time operation as each vertex only has two adjacent edges

        auto barrier_polyline_vertices = std::vector<Coordinate>();
        barrier_polyline_vertices.reserve(3);

        const auto &vertex_segments = vertices_and_segments.at(observer_coordinate);
        for (size_t i = 0; i < vertex_segments.size(); ++i) {
            barrier_polyline_vertices.push_back(vertex_segments[i].get_endpoint_1());
            if (i != 0)
                barrier_polyline_vertices.push_back(vertex_segments[i].get_endpoint_2());
        }

        if (barrier_polyline_vertices.size() != 3) {
            throw std::runtime_error("Barrier polyline vertices unexpectedly does not consist of three vertices");
        }

        const auto barrier_polyline = ThreeVertexPolyline(barrier_polyline_vertices[0], barrier_polyline_vertices[1],
                                                          barrier_polyline_vertices[2]);
        if (!barrier_polyline.point_visible(vertex_in_question)) {
            return false;
        }
    }

    if (!open_edges.empty()) {
        const auto &closest_edge = open_edges.begin()->second;
        const auto intersection =
            closest_edge->intersection_with_segment(LineSegment(observer_coordinate, vertex_in_question));
        if (intersection.has_value()) {
            return false;
        }
    }

    return true;
}

bool VistreeGenerator::is_vertex_visible(const VistreeGenerator::OpenEdges &open_edges,
                                         const VistreeGenerator::VertexToSegmentMapping &vertices_and_segments,
                                         const Coordinate &observer_coordinate, const Coordinate &vertex_in_question,
                                         const std::optional<Coordinate> &prev_vertex, bool prev_vertex_visible) {
    if (prev_vertex.has_value() &&
        LineSegment(observer_coordinate, vertex_in_question).orientation_of_point_to_segment(prev_vertex.value()) ==
            Orientation::COLLINEAR) {
        return VistreeGenerator::is_vertex_visible_collinear(open_edges, prev_vertex_visible, prev_vertex,
                                                             vertex_in_question);
    }

    return VistreeGenerator::is_vertex_visible_non_collinear(open_edges, vertices_and_segments, observer_coordinate,
                                                             vertex_in_question);
}

void VistreeGenerator::erase_segments_from_open_edges(const std::vector<LineSegment> &segments,
                                                      VistreeGenerator::OpenEdges &open_edges) {
    for (const auto &segment : segments) {
        for (auto iter = open_edges.begin(); iter != open_edges.end(); ++iter) {
            if (*(iter->second) == segment) {
                open_edges.erase(iter);
                break;
            }
        }
    }
}

void VistreeGenerator::add_segments_to_open_edges(const std::vector<LineSegment> &segments,
                                                  VistreeGenerator::OpenEdges &open_edges, const Coordinate &observer,
                                                  const Coordinate &current_vertex) {
    for (const auto &segment : segments) {
        const auto intersection = segment.intersection_with_segment(LineSegment(observer, current_vertex));
        const auto distance_squared = (intersection.value() - observer).magnitude_squared();

        open_edges.emplace(distance_squared, std::make_unique<LineSegment>(segment));
    }
}
