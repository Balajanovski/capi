//
// Created by James.Balajan on 31/03/2021.
//

#ifndef CAPI_LINE_SEGMENT_HPP
#define CAPI_LINE_SEGMENT_HPP

#include <optional>

#include "types/coordinate/coordinate.hpp"

class LineSegment {
  public:
    LineSegment(const Coordinate &endpoint_1, const Coordinate &endpoint_2)
        : _endpoint_1(endpoint_1), _endpoint_2(endpoint_2) {}

    [[nodiscard]] Coordinate get_endpoint_1() const;
    [[nodiscard]] Coordinate get_endpoint_2() const;
    [[nodiscard]] std::optional<Coordinate> intersection_with_ray(const Coordinate& ray_start, const Coordinate& ray_vector) const;

    bool operator==(const LineSegment& other) const;
    bool operator!=(const LineSegment& other) const;
  private:
    Coordinate _endpoint_1;
    Coordinate _endpoint_2;
};

#endif // CAPI_LINE_SEGMENT_HPP
