//
// Created by James.Balajan on 31/03/2021.
//


#include <catch.hpp>
#include <vector>

#include "types/coordinate.hpp"
#include "types/polygon.hpp"


TEST_CASE("Polygon Constructor")
{
    const auto polygon = Polygon({Coordinate(1, 2),
                                  Coordinate(2, 1),
                                  Coordinate(3, 3)});

    REQUIRE(polygon.get_vertices().size() == 3);
    REQUIRE(polygon.get_vertices() == std::vector<Coordinate>({Coordinate(1, 2),
                                                   Coordinate(2, 1),
                                                   Coordinate(3, 3)}));
}

