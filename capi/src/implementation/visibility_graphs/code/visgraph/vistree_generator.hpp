//
// Created by James.Balajan on 6/04/2021.
//

#ifndef CAPI_VISTREE_GENERATOR_HPP
#define CAPI_VISTREE_GENERATOR_HPP

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "types/coordinate/coordinate.hpp"
#include "types/line_segment/line_segment.hpp"
#include "types/polygon/polygon.hpp"
#include "types/visible_vertex/visible_vertex.hpp"
#include "datastructures/open_edges/open_edges.hpp"

class VistreeGenerator {
  public:
    static std::vector<VisibleVertex> get_visible_vertices_from_root(const Coordinate &observer,
                                                                     const std::vector<Polygon> &polygons,
                                                                     bool half_scan = false);

  private:
    using VertexToSegmentMapping = std::unordered_map<Coordinate, std::vector<std::shared_ptr<LineSegment>>>;

    static VertexToSegmentMapping all_vertices_and_incident_segments(const std::vector<Polygon> &polygons);
    static std::vector<std::shared_ptr<LineSegment>> all_line_segments(const VertexToSegmentMapping &vertices_and_segments);
    static std::vector<Coordinate> all_vertices(const VertexToSegmentMapping &vertices_and_segments);
    static std::vector<std::shared_ptr<LineSegment>> orientation_segments(const std::vector<std::shared_ptr<LineSegment>> &segments,
                                                         const LineSegment &scanline_segment,
                                                         const Orientation &desired_orientation);

    static bool is_vertex_visible(const OpenEdges &open_edges,
                                  const VertexToSegmentMapping &vertices_and_segments,
                                  const Coordinate &observer_coordinate,
                                  const Coordinate &vertex_in_question);

    static void erase_segments_from_open_edges(const std::vector<std::shared_ptr<LineSegment>> &segments, OpenEdges &open_edges);
    static void add_segments_to_open_edges(const std::vector<std::shared_ptr<LineSegment>> &segments, OpenEdges &open_edges,
                                           const Coordinate &observer, const Coordinate &current_vertex);
};

#endif // CAPI_VISTREE_GENERATOR_HPP
