//
// Created by James.Balajan on 6/04/2021.
//

#ifndef CAPI_VISTREE_GENERATOR_HPP
#define CAPI_VISTREE_GENERATOR_HPP

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "datastructures/open_edges/open_edges.hpp"
#include "types/coordinate/coordinate.hpp"
#include "types/line_segment/line_segment.hpp"
#include "types/polygon/polygon.hpp"
#include "types/visible_vertex/visible_vertex.hpp"

class VistreeGenerator {
  public:
    explicit VistreeGenerator(const std::vector<Polygon> &polygons);
    explicit VistreeGenerator(const std::vector<std::shared_ptr<LineSegment>> &segments);
    [[nodiscard]] std::vector<VisibleVertex> get_visible_vertices(const Coordinate &observer,
                                                                  bool half_scan = false) const;
    [[nodiscard]] std::vector<VisibleVertex>
    get_visible_vertices_from_candidate_segments(const Coordinate &observer,
                                                 const std::vector<std::shared_ptr<LineSegment>> &candidate_segments,
                                                 bool half_scan = false) const;

  private:
    using VertexToSegmentMapping = std::unordered_map<Coordinate, std::vector<std::shared_ptr<LineSegment>>>;

    static VertexToSegmentMapping all_vertices_and_incident_segments(const std::vector<Polygon> &polygons);
    static std::vector<std::shared_ptr<LineSegment>>
    orientation_segments(const std::vector<std::shared_ptr<LineSegment>> &segments, const LineSegment &scanline_segment,
                         const Orientation &desired_orientation);

    static void erase_segments_from_open_edges(const std::vector<std::shared_ptr<LineSegment>> &segments,
                                               OpenEdges &open_edges);
    static void add_segments_to_open_edges(const std::vector<std::shared_ptr<LineSegment>> &segments,
                                           OpenEdges &open_edges, const Coordinate &observer,
                                           const Coordinate &current_vertex);

    static std::vector<Coordinate>
    all_vertices_for_line_segments(const std::vector<std::shared_ptr<LineSegment>> &line_segments);

    [[nodiscard]] std::vector<VisibleVertex> get_visible_vertices_from_candidate_segments_and_vertices(
        const Coordinate &observer, const std::vector<Coordinate> &candidate_vertices,
        const std::vector<std::shared_ptr<LineSegment>> &candidate_segments, bool half_scan) const;
    [[nodiscard]] std::vector<std::shared_ptr<LineSegment>> all_line_segments() const;
    [[nodiscard]] std::vector<Coordinate> all_vertices() const;
    [[nodiscard]] bool is_vertex_visible(const OpenEdges &open_edges, const Coordinate &observer_coordinate,
                                         const Coordinate &vertex_in_question) const;

    VertexToSegmentMapping _vertices_and_segments;
};

#endif // CAPI_VISTREE_GENERATOR_HPP
