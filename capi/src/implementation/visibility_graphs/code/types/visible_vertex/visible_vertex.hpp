//
// Created by James.Balajan on 19/05/2021.
//

#ifndef CAPI_VISIBLE_VERTEX_HPP
#define CAPI_VISIBLE_VERTEX_HPP

#include "types/coordinate/coordinate.hpp"

struct VisibleVertex {
    Coordinate coord;
    bool is_visible_across_meridian;

    bool operator==(const VisibleVertex &other) const;
    bool operator!=(const VisibleVertex &other) const;
};

std::ostream &operator<<(std::ostream &outs, const VisibleVertex &coord);

#endif // CAPI_VISIBLE_VERTEX_HPP
