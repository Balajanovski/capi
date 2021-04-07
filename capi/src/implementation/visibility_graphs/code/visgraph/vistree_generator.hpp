//
// Created by James.Balajan on 6/04/2021.
//

#ifndef CAPI_VISTREE_GENERATOR_HPP
#define CAPI_VISTREE_GENERATOR_HPP

#include <vector>
#include <unordered_map>
#include <map>
#include <optional>
#include <memory>

#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"
#include "types/line_segment/line_segment.hpp"

class VistreeGenerator {
public:
    static std::vector<Coordinate> get_visible_vertices_from_root(const Coordinate& nudged_observer, const std::vector<Polygon>& polygons);
private:
    using OpenEdges = std::multimap<double, const std::unique_ptr<LineSegment>>;
    using VertexToSegmentMapping = std::unordered_map<Coordinate, std::vector<LineSegment>>;

    static VertexToSegmentMapping all_vertices_and_incident_segments(const std::vector<Polygon>& polygons);
    static std::vector<LineSegment> all_line_segments(const VertexToSegmentMapping& vertices_and_segments);
    static std::vector<Coordinate> all_vertices(const VertexToSegmentMapping& vertices_and_segments);
    static std::vector<LineSegment> clockwise_segments(const std::vector<LineSegment>& segments, const Coordinate& ray_base, const Coordinate& ray_direction);
    static std::vector<LineSegment> counter_clockwise_segments(const std::vector<LineSegment>& segments, const Coordinate& ray_base, const Coordinate& ray_direction);
    static std::vector<LineSegment> collinear_segments(const std::vector<LineSegment>& segments, const Coordinate& ray_base, const Coordinate& ray_direction);
    static bool is_vertex_visible(const OpenEdges& open_edges, const Coordinate& observer_coordinate, const Coordinate& vertex_in_question, const std::optional<Coordinate>& prev_vertex, const std::optional<bool>& prev_vertex_visible);
    static void erase_segments_from_open_edges(const std::vector<LineSegment>& segments, OpenEdges& open_edges);
    static void add_segments_to_open_edges(const std::vector<LineSegment>& segments, OpenEdges& open_edges, const Coordinate& observer, const Coordinate& current_vertex);
    static Coordinate nudge_observer_out_of_polygon(const Coordinate& observer, const VertexToSegmentMapping& vertices_and_segments);
};


#endif //CAPI_VISTREE_GENERATOR_HPP
