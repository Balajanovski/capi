//
// Created by James.Balajan on 6/04/2021.
//

#include "line_segment_orientation_to_ray.hpp"

Orientation line_segment_orientation_to_ray(const LineSegment& segment, const Coordinate& ray_base, const Coordinate& ray_direction) {
    const auto endpoint_1_orientation = ray_direction.vector_orientation(segment.get_endpoint_1() - ray_base);
    const auto endpoint_2_orientation = ray_direction.vector_orientation(segment.get_endpoint_2() - ray_base);
    return static_cast<Orientation>(endpoint_1_orientation & endpoint_2_orientation);
}
