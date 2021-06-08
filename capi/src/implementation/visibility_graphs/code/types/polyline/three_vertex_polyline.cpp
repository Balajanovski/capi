//
// Created by James.Balajan on 13/04/2021.
//

#include <cmath>
#include <stdexcept>

#include "constants/constants.hpp"
#include "three_vertex_polyline.hpp"

ThreeVertexPolyline::ThreeVertexPolyline(Coordinate v1, Coordinate v2, Coordinate v3) : _v1(v1), _v2(v2), _v3(v3) {}

bool ThreeVertexPolyline::point_visible(const Coordinate &coordinate) const {
    Coordinate vec_1 = _v1 - _v2;
    Coordinate vec_2 = _v3 - _v2;
    Coordinate vec_3 = coordinate - _v2;

    const auto reflex = is_reflex();

    /*
     * We use Cramer's rule to determine if we can solve
     * vec_3 = lambda_1 * vec_1 + lambda_2 * vec_2 where lambda_1, lambda_2 >= 0
     * if we can solve this the point is obstructed
     */

    const auto determinant =
        (vec_1.get_longitude() * vec_2.get_latitude()) - (vec_1.get_latitude() * vec_2.get_longitude());
    const auto lambda_1 =
        ((vec_3.get_longitude() * vec_2.get_latitude()) - (vec_3.get_latitude() * vec_2.get_longitude())) / determinant;
    const auto lambda_2 =
        ((vec_1.get_longitude() * vec_3.get_latitude()) - (vec_1.get_latitude() * vec_3.get_longitude())) / determinant;

    if (!reflex) {
        return !(lambda_1 > EPSILON_TOLERANCE_SQUARED && lambda_2 > EPSILON_TOLERANCE_SQUARED);
    } else {
        return (lambda_1 > -EPSILON_TOLERANCE_SQUARED && lambda_2 > -EPSILON_TOLERANCE_SQUARED);
    }
}

bool ThreeVertexPolyline::is_reflex() const {
    // This makes the assumption that the polyline's vertices are from a counter clockwise wound polygon

    const auto s = _v2 - _v1;
    const auto t = _v3 - _v2;

    const auto determinant = (s.get_longitude() * t.get_latitude()) - (s.get_latitude() * t.get_longitude());
    return determinant < 0;
}
