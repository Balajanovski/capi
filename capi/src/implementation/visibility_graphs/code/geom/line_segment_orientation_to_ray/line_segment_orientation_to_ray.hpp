//
// Created by James.Balajan on 6/04/2021.
//

#ifndef CAPI_LINE_SEGMENT_ORIENTATION_TO_RAY_HPP
#define CAPI_LINE_SEGMENT_ORIENTATION_TO_RAY_HPP

#include "types/orientation/orientation.hpp"
#include "types/line_segment/line_segment.hpp"
#include "types/coordinate/coordinate.hpp"

Orientation line_segment_orientation_to_ray(const LineSegment& segment, const Coordinate& ray_base, const Coordinate& ray_direction);

#endif //CAPI_LINE_SEGMENT_ORIENTATION_TO_RAY_HPP
