//
// Created by James.Balajan on 31/03/2021.
//


#include <catch.hpp>

#include "math.hpp"

TEST_CASE("Addition and subtraction")
{
    REQUIRE(add(1, 1) == 2);
    REQUIRE(subtract(1, 1) == 0);
}
