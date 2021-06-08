//
// Created by James.Balajan on 18/05/2021.
//

#ifndef CAPI_COORDINATE_PERIODICITY_HPP
#define CAPI_COORDINATE_PERIODICITY_HPP

#include <memory>
#include <unordered_map>
#include <vector>

#include "types/coordinate/coordinate.hpp"
#include "types/line_segment/line_segment.hpp"
#include "types/polygon/polygon.hpp"

std::vector<Coordinate> periodic_coordinates_from_coordinate(const Coordinate &coordinate);
bool is_coordinate_over_meridian(const Coordinate &coordinate);
Coordinate coordinate_from_periodic_coordinate(const Coordinate &periodic_coordinate);
std::vector<Polygon> make_polygons_periodic(const std::vector<Polygon> &polygons);
std::vector<Coordinate> make_coordinates_periodic(const std::vector<Coordinate> &coordinates);
std::vector<std::shared_ptr<LineSegment>> make_segments_periodic(const std::vector<LineSegment> &segments);

#endif // CAPI_COORDINATE_PERIODICITY_HPP
