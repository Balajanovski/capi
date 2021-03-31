//
// Created by James.Balajan on 31/03/2021.
//


#include <catch.hpp>

#include "types/coordinate/coordinate.hpp"


TEST_CASE("Coordinate Constructor")
{
    const auto coord = Coordinate(1, 2);
    REQUIRE(coord.get_longitude() == 1);
    REQUIRE(coord.get_latitude() == 2);
}


TEST_CASE("Coordinate Equals")
{
    const auto coord1 = Coordinate(1, 2);
    const auto coord2 = Coordinate(1, 2);
    const auto coord3 = Coordinate(2, 1);

    REQUIRE(coord1 == coord2);
    REQUIRE_FALSE(coord1 != coord2);

    REQUIRE_FALSE(coord1 == coord3);
    REQUIRE(coord1 != coord3);
}
