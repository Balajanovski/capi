//
// Created by James.Balajan on 31/03/2021.
//

#include <catch.hpp>

#include "geom/left_turn/left_turn.hpp"
#include "types/coordinate/coordinate.hpp"

TEST_CASE("Point on Left Side") {
    const auto p = Coordinate(1, 0);
    const auto q = Coordinate(2, 3);
    const auto r = Coordinate(-1, 1);

    REQUIRE(left_turn(p, q, r));
}

TEST_CASE("Point on Right Side") {
    const auto p = Coordinate(1, 0);
    const auto q = Coordinate(2, 3);
    const auto r = Coordinate(2, 0);

    REQUIRE_FALSE(left_turn(p, q, r));
}
