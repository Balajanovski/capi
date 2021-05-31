//
// Created by James.Balajan on 13/04/2021.
//

#include <catch.hpp>
#include <cmath>

#include "geom/fast_arctan/fast_arctan.hpp"

TEST_CASE("Fast arctan approximates arc tan") {
    const auto num_subdivisions = 10000;
    const auto subdiv_angle = (2 * M_PI) / num_subdivisions;
    const auto epsilon_tolerance = 0.01;

    for (int i = 0; i < num_subdivisions; ++i) {
        const auto angle = (subdiv_angle)*i;

        if (std::abs(M_PI_2 - angle) <= epsilon_tolerance || std::abs((3 * M_PI_2) - angle) <= epsilon_tolerance) {
            continue;
        }

        const auto atan_res = atan(angle);
        const auto fast_atan_res = fast_arctan(angle);

        REQUIRE(std::abs(atan_res - fast_atan_res) <= epsilon_tolerance);
    }
}
