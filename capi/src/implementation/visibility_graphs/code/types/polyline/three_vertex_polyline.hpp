//
// Created by James.Balajan on 13/04/2021.
//

#ifndef CAPI_THREE_VERTEX_POLYLINE_HPP
#define CAPI_THREE_VERTEX_POLYLINE_HPP

#include <initializer_list>
#include <vector>

#include "types/coordinate/coordinate.hpp"
#include "types/orientation/orientation.hpp"

class ThreeVertexPolyline {
  public:
    ThreeVertexPolyline(Coordinate v1, Coordinate v2, Coordinate v3);

    [[nodiscard]] bool point_visible(const Coordinate &coordinate) const;
    [[nodiscard]] bool is_reflex() const;

  private:
    Coordinate _v1;
    Coordinate _v2;
    Coordinate _v3;
};

#endif // CAPI_THREE_VERTEX_POLYLINE_HPP
