//
// Created by James.Balajan on 6/04/2021.
//

#ifndef CAPI_VISTREE_GENERATOR_HPP
#define CAPI_VISTREE_GENERATOR_HPP

#include <vector>
#include <unordered_map>

#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"
#include "types/line_segment/line_segment.hpp"

class VistreeGenerator {
public:
    static std::vector<Coordinate> get_visible_vertices_from_root(const Coordinate& root_vertex, const std::vector<Polygon>& polygons);
private:
    static std::unordered_map<Coordinate, std::vector<LineSegment>> all_vertices_and_incident_segments(const std::vector<Polygon>& polygons);
    static std::vector<LineSegment> all_line_segments(const std::unordered_map<Coordinate, std::vector<LineSegment>>& vertices_and_segments);
    static std::vector<Coordinate> all_vertices_excluding_root(const std::unordered_map<Coordinate, std::vector<LineSegment>>& vertices_and_segments, const Coordinate& root);
};


#endif //CAPI_VISTREE_GENERATOR_HPP
