//
// Created by James.Balajan on 13/04/2021.
//

#include "types/polygon/polygon.hpp"
#include "three_vertex_polyline.hpp"
#include "constants/constants.hpp"

ThreeVertexPolyline::ThreeVertexPolyline(Coordinate v1, Coordinate v2, Coordinate v3) : _v1(v1), _v2(v2), _v3(v3) {}

bool ThreeVertexPolyline::point_visible(const Coordinate &coordinate) const {
    const auto v2v1 = _v1 - _v2;
    const auto v2v3 = _v3 - _v2;
    const auto v2coord = coordinate - _v2;

    const auto v2v1_orientation = v2v1.vector_orientation(v2coord);
    const auto v2v3_orientation = v2v3.vector_orientation(v2coord);
    const auto reflex = is_reflex();

    if (!reflex) {
        return !(
            (v2v1_orientation == Orientation::CLOCKWISE && v2v3_orientation == Orientation::COUNTER_CLOCKWISE) ||
            (v2v1_orientation == Orientation::COLLINEAR && v2v3_orientation == Orientation::COUNTER_CLOCKWISE &&
                    !ThreeVertexPolyline::inclusive_between(v2v1.scalar_multiple_factor(v2coord).value(), 0, 1)) ||
            (v2v1_orientation == Orientation::CLOCKWISE && v2v3_orientation == Orientation::COLLINEAR &&
                    !ThreeVertexPolyline::inclusive_between(v2v3.scalar_multiple_factor(v2coord).value(), 0, 1))
        );
    } else {
        return (
            (v2v1_orientation == Orientation::COUNTER_CLOCKWISE && v2v3_orientation == Orientation::CLOCKWISE) ||
            (v2v1_orientation == Orientation::COLLINEAR && v2v3_orientation == Orientation::CLOCKWISE &&
                    ThreeVertexPolyline::inclusive_between(v2v1.scalar_multiple_factor(v2coord).value(), 0, 1)) ||
            (v2v1_orientation == Orientation::COUNTER_CLOCKWISE && v2v3_orientation == Orientation::COLLINEAR &&
                    ThreeVertexPolyline::inclusive_between(v2v3.scalar_multiple_factor(v2coord).value(), 0, 1))
        );
    }
}

bool ThreeVertexPolyline::is_reflex() const {
    // This makes the assumption that the polyline's vertices are from a counter clockwise wound polygon

    const auto s = _v2 - _v1;
    const auto t = _v3 - _v2;

    const auto determinant = (s.get_longitude() * t.get_latitude()) - (s.get_latitude() * t.get_longitude());
    return determinant < 0;
}

bool ThreeVertexPolyline::inclusive_between(double value, double lower, double upper) {
    return (value >= lower - EPSILON_TOLERANCE) && (value <= upper + EPSILON_TOLERANCE);
}
