//
// Created by James.Balajan on 19/05/2021.
//

#include "visible_vertex.hpp"

bool VisibleVertex::operator==(const VisibleVertex &other) const {
    return is_visible_across_meridian == other.is_visible_across_meridian && coord == other.coord;
}

bool VisibleVertex::operator!=(const VisibleVertex &other) const { return !((*this) == other); }

std::ostream &operator<<(std::ostream &outs, const VisibleVertex &coord) {
    return outs << '{' << coord.coord << ", " << coord.is_visible_across_meridian << '}';
}
