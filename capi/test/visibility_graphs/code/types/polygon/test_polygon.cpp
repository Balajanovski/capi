//
// Created by James.Balajan on 31/03/2021.
//

#include <catch.hpp>
#include <vector>

#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"

TEST_CASE("Polygon Constructor") {
    const auto polygon = Polygon({Coordinate(1, 2), Coordinate(2, 1), Coordinate(3, 3)});

    REQUIRE(polygon.get_vertices().size() == 3);
    REQUIRE(polygon.get_vertices() == std::vector<Coordinate>({Coordinate(1, 2), Coordinate(2, 1), Coordinate(3, 3)}));
}

TEST_CASE("Polygon Equals") {
    const auto vertices1 = std::vector<Coordinate>{Coordinate(1, 2), Coordinate(2, 1), Coordinate(3, 3)};
    const auto vertices2 = std::vector<Coordinate>{Coordinate(1, 2), Coordinate(2, 2), Coordinate(1, 1)};

    const auto poly1 = Polygon(vertices1);
    const auto poly2 = Polygon(vertices1);
    const auto poly3 = Polygon(vertices2);

    REQUIRE(poly1 == poly2);
    REQUIRE_FALSE(poly1 != poly2);

    REQUIRE_FALSE(poly1 == poly3);
    REQUIRE(poly1 != poly3);
}
