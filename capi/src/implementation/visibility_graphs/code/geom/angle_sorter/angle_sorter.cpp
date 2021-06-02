//
// Created by James.Balajan on 6/04/2021.
//

#include <algorithm>

#include "angle_sorter.hpp"

void AngleSorter::sort_counter_clockwise_around_observer(const Coordinate &observer,
                                                         std::vector<Coordinate> &vertices) {
    if (vertices.empty()) {
        return;
    }

    const auto cmp = [&](const Coordinate &c1, const Coordinate &c2) -> bool {
        if (c1 == observer) {
            return true;
        } else if (c2 == observer) {
            return false;
        }

        const auto v1 = c1 - observer;
        const auto v2 = c2 - observer;
        const auto dotprod = v1.dot_product_microdegrees(v2);

        if (v1.vector_orientation(v2) != Orientation::COLLINEAR || dotprod < 0) {
            const auto v1_angle_to_horizontal = v1.angle_to_horizontal();
            const auto v2_angle_to_horizontal = v2.angle_to_horizontal();

            return v1_angle_to_horizontal < v2_angle_to_horizontal;
        }

        return v1.magnitude_squared() < v2.magnitude_squared();
    };

    std::sort(vertices.begin(), vertices.end(), cmp);
}
