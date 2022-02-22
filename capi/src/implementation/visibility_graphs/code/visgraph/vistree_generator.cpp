//
// Created by James.Balajan on 6/04/2021.
//

#include <stdexcept>
#include <unordered_set>

#include "constants/constants.hpp"
#include "coordinate_periodicity/coordinate_periodicity.hpp"
#include "geom/angle_sorter/angle_sorter.hpp"
#include "types/polyline/three_vertex_polyline.hpp"
#include "vistree_generator.hpp"

VistreeGenerator::VistreeGenerator(const std::vector<Polygon> &polygons)
    : _vertices_and_segments(VistreeGenerator::all_vertices_and_incident_segments(polygons)) {}

VistreeGenerator::VistreeGenerator(const std::vector<std::shared_ptr<LineSegment>> &segments) {
    _vertices_and_segments = VistreeGenerator::VertexToSegmentMapping();

    for (const auto &segment : segments) {
        const auto p1 = segment->get_endpoint_1();
        const auto p2 = segment->get_endpoint_2();

        if (_vertices_and_segments.find(p1) == _vertices_and_segments.end()) {
            _vertices_and_segments[p1] = std::vector<std::shared_ptr<LineSegment>>{};
        } if (_vertices_and_segments.find(p2) == _vertices_and_segments.end()) {
            _vertices_and_segments[p2] = std::vector<std::shared_ptr<LineSegment>>{};
        }

        _vertices_and_segments[p1].push_back(segment);
        _vertices_and_segments[p2].push_back(segment);
    }
}

std::vector<VisibleVertex> VistreeGenerator::get_visible_vertices(const Coordinate &observer, bool half_scan) const {
    return get_visible_vertices_from_candidate_segments_and_vertices(observer, VistreeGenerator::all_vertices(),
                                                                     VistreeGenerator::all_line_segments(), half_scan);
}

std::vector<VisibleVertex> VistreeGenerator::get_visible_vertices_from_candidate_segments(
    const Coordinate &observer, const std::vector<std::shared_ptr<LineSegment>> &candidate_segments,
    bool half_scan) const {
    return get_visible_vertices_from_candidate_segments_and_vertices(
        observer, all_vertices_for_line_segments(candidate_segments), candidate_segments, half_scan);
}

std::vector<VisibleVertex> VistreeGenerator::get_visible_vertices_from_candidate_segments_and_vertices(
    const Coordinate &observer, const std::vector<Coordinate> &candidate_vertices,
    const std::vector<std::shared_ptr<LineSegment>> &candidate_segments, bool half_scan) const {
    if (_vertices_and_segments.empty()) {
        return {};
    }

    auto vertices_sorted_counter_clockwise_around_observer = candidate_vertices;
    AngleSorter::sort_counter_clockwise_around_observer(observer, vertices_sorted_counter_clockwise_around_observer);

    auto open_edges = OpenEdges();
    const auto initial_scanline_segment =
        LineSegment(observer, Coordinate(MAX_PERIODIC_LONGITUDE, observer.get_latitude()));
    const auto initial_scanline_vector = initial_scanline_segment.get_tangent_vector();
    for (const auto &line_segment : candidate_segments) {
        if (observer == line_segment->get_endpoint_1() || observer == line_segment->get_endpoint_2()) {
            continue;
        }

        const auto intersection = line_segment->intersection_with_segment(initial_scanline_segment);
        if (intersection.has_value() && !initial_scanline_segment.on_segment(line_segment->get_endpoint_1()) &&
            !initial_scanline_segment.on_segment(line_segment->get_endpoint_2())) {
            open_edges.add_edge((intersection.value() - observer).magnitude_squared_microdegrees(), *line_segment);
        }
    }

    std::vector<VisibleVertex> visible_vertices;
    for (const auto &current_vertex : vertices_sorted_counter_clockwise_around_observer) {
        if (observer == current_vertex) {
            continue;
        }

        const auto scanline_segment = LineSegment(observer, current_vertex);
        if (half_scan &&
            initial_scanline_vector.cross_product_magnitude_microdegrees(scanline_segment.get_tangent_vector()) < 0) {
            break;
        }

        const auto &incident_segments = _vertices_and_segments.at(current_vertex);
        const auto clockwise_segments =
            VistreeGenerator::orientation_segments(incident_segments, scanline_segment, Orientation::CLOCKWISE);
        const auto counter_clockwise_segments =
            VistreeGenerator::orientation_segments(incident_segments, scanline_segment, Orientation::COUNTER_CLOCKWISE);

        VistreeGenerator::erase_segments_from_open_edges(clockwise_segments, open_edges);

        const auto curr_vertex_visible = VistreeGenerator::is_vertex_visible(open_edges, observer, current_vertex);
        if (curr_vertex_visible) {
            visible_vertices.push_back(VisibleVertex{
                    .coord = coordinate_from_periodic_coordinate(current_vertex),
                    .is_visible_across_meridian = is_coordinate_over_meridian(current_vertex),
            });
        }

        VistreeGenerator::add_segments_to_open_edges(counter_clockwise_segments, open_edges, observer, current_vertex);
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

            vertices_and_segments[curr_vertex] = std::vector<std::shared_ptr<LineSegment>>{
                std::make_shared<LineSegment>(prev_vertex, curr_vertex),
                std::make_shared<LineSegment>(curr_vertex, next_vertex),
            };
        }
    }

    return vertices_and_segments;
}

std::vector<std::shared_ptr<LineSegment>> VistreeGenerator::all_line_segments() const {
    std::unordered_set<std::shared_ptr<LineSegment>> segments;
    segments.reserve(_vertices_and_segments.size());

    for (const auto &vertex_and_segments : _vertices_and_segments) {
        for (const auto &segment : vertex_and_segments.second) {
            segments.insert(segment);
        }
    }

    return std::vector<std::shared_ptr<LineSegment>>(segments.begin(), segments.end());
}

std::vector<Coordinate> VistreeGenerator::all_vertices() const {
    std::vector<Coordinate> vertices;
    vertices.reserve(_vertices_and_segments.size());

    for (const auto &vertex_and_segments : _vertices_and_segments) {
        vertices.push_back(vertex_and_segments.first);
    }

    return vertices;
}

std::vector<Coordinate>
VistreeGenerator::all_vertices_for_line_segments(const std::vector<std::shared_ptr<LineSegment>> &line_segments) {
    auto vertices = std::unordered_set<Coordinate>();

    for (const auto &segment : line_segments) {
        vertices.insert(segment->get_endpoint_1());
        vertices.insert(segment->get_endpoint_2());
    }

    return std::vector<Coordinate>(vertices.begin(), vertices.end());
}

std::vector<std::shared_ptr<LineSegment>>
VistreeGenerator::orientation_segments(const std::vector<std::shared_ptr<LineSegment>> &segments,
                                       const LineSegment &scanline_segment, const Orientation &desired_orientation) {
    std::vector<std::shared_ptr<LineSegment>> orientation_segs;

    for (const auto &segment : segments) {
        const auto adjacent_to_segment_point = segment->get_adjacent_to(scanline_segment.get_endpoint_2());
        const auto segment_orientation = scanline_segment.orientation_of_point_to_segment(adjacent_to_segment_point);
        if (segment_orientation == desired_orientation) {
            orientation_segs.push_back(segment);
        }
    }

    return orientation_segs;
}

bool VistreeGenerator::is_vertex_visible(const OpenEdges &open_edges, const Coordinate &observer_coordinate,
                                         const Coordinate &vertex_in_question) const {
    if (vertex_in_question == observer_coordinate) {
        return false;
    }
    if (_vertices_and_segments.find(observer_coordinate) != _vertices_and_segments.end()) {
        // We perform this check to stop our observer coordinate (if it is a vertex of a polygon)
        // from seeing vertices from inside the polygon
        // Essentially we build artificial walls based on the edges we know obstruct vision (those adjacent)
        // This will be a constant time operation as each vertex only has two adjacent edges

        auto barrier_polyline_vertices = std::vector<Coordinate>();
        barrier_polyline_vertices.reserve(3);

        const auto &vertex_segments = _vertices_and_segments.at(observer_coordinate);
        for (size_t i = 0; i < vertex_segments.size(); ++i) {
            barrier_polyline_vertices.push_back(vertex_segments[i]->get_endpoint_1());
            if (i != 0)
                barrier_polyline_vertices.push_back(vertex_segments[i]->get_endpoint_2());
        }

        const auto barrier_polyline = ThreeVertexPolyline(barrier_polyline_vertices[0], barrier_polyline_vertices[1],
                                                          barrier_polyline_vertices[2]);
        if (!barrier_polyline.point_visible(vertex_in_question)) {
            return false;
        }
    }

    if (!open_edges.empty()) {
        const auto &closest_edge = open_edges.closest_edge();
        const auto intersection =
            closest_edge.intersection_with_segment(LineSegment(observer_coordinate, vertex_in_question));
        if (intersection.has_value() && intersection.value() != vertex_in_question) {
            return false;
        }
    }

    return true;
}

void VistreeGenerator::erase_segments_from_open_edges(const std::vector<std::shared_ptr<LineSegment>> &segments,
                                                      OpenEdges &open_edges) {
    for (const auto &segment : segments) {
        open_edges.remove_edge(*segment);
    }
}

void VistreeGenerator::add_segments_to_open_edges(const std::vector<std::shared_ptr<LineSegment>> &segments,
                                                  OpenEdges &open_edges, const Coordinate &observer,
                                                  const Coordinate &current_vertex) {
    for (const auto &segment : segments) {
        const auto intersection = segment->intersection_with_segment(LineSegment(observer, current_vertex));
        const auto distance_squared = (intersection.value() - observer).magnitude_squared_microdegrees();

        open_edges.add_edge(distance_squared, *segment);
    }
}
