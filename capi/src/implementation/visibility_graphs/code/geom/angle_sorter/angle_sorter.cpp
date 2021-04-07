//
// Created by James.Balajan on 6/04/2021.
//

#include <algorithm>

#include "angle_sorter.hpp"


void AngleSorter::sort_counter_clockwise_around_observer(const Coordinate &observer, std::vector<Coordinate> &vertices) {
    if (vertices.empty()) {
        return;
    }

    const auto cmp = [&](const Coordinate& c1, const Coordinate& c2) -> bool {
        if (c1 == observer) {
            return true;
        } else if (c2 == observer) {
            return false;
        }

        const auto v1 = c1 - observer;
        const auto v2 = c2 - observer;

        if (v1.get_longitude() >= 0 && v2.get_longitude() < 0) {
            return false;
        } if (v1.get_longitude() < 0 && v2.get_longitude() >= 0) {
            return true;
        } if (v1.get_longitude() == 0 && v2.get_longitude() == 0) {
            if (v1.get_latitude() >= 0 || v2.get_latitude() >= 0) {
                return v1.get_latitude() < v2.get_latitude();
            }
            return v2.get_latitude() < v1.get_latitude();
        }

        const auto det = v1.cross_product_magnitude(v2);
        if (det < 0) return false;
        if (det > 0) return true;

        return v1.magnitude_squared() > v2.magnitude_squared();
    };

    std::sort(vertices.begin(), vertices.end(), cmp);
}
