//
// Created by James.Balajan on 31/03/2021.
//

#include <catch.hpp>
#include <vector>

#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"

TEST_CASE("Polygon Constructor Counter Clockwise") {
    const auto polygon = Polygon({Coordinate(1., 2.), Coordinate(2., 1.), Coordinate(3., 3.)});

    REQUIRE(polygon.get_vertices().size() == 3);
    REQUIRE(polygon.get_vertices() ==
            std::vector<Coordinate>({Coordinate(1., 2.), Coordinate(2., 1.), Coordinate(3., 3.)}));
}

TEST_CASE("Polygon Constructor Clockwise") {
    const auto polygon = Polygon({Coordinate(3., 3.), Coordinate(2., 1.), Coordinate(1., 2.)});

    REQUIRE(polygon.get_vertices().size() == 3);
    REQUIRE(polygon.get_vertices() ==
            std::vector<Coordinate>({Coordinate(1., 2.), Coordinate(2., 1.), Coordinate(3., 3.)}));
}

TEST_CASE("Polygon Constructor Collinear : Line") {
    const auto polygon = Polygon({Coordinate(0., 0.), Coordinate(1., 1.)});

    REQUIRE(polygon.get_vertices().size() == 2);
    REQUIRE(polygon.get_vertices() == std::vector<Coordinate>({Coordinate(0., 0.), Coordinate(1., 1.)}));
}

TEST_CASE("Polygon Constructor Collinear : Line 2") {
    const auto polygon = Polygon({Coordinate(0., 0.), Coordinate(0.5, 0.5), Coordinate(1., 1.)});

    REQUIRE(polygon.get_vertices().size() == 2);
    REQUIRE(polygon.get_vertices() == std::vector<Coordinate>({Coordinate(0., 0.), Coordinate(1., 1.)}));
}

TEST_CASE("Polygon Constructor Collinear : Triangle") {
    const auto polygon = Polygon({Coordinate(0., 0.), Coordinate(1., 0.), Coordinate(0., 1.), Coordinate(-1., 0.)});

    REQUIRE(polygon.get_vertices().size() == 3);
    REQUIRE(polygon.get_vertices() ==
            std::vector<Coordinate>({Coordinate(1., 0.), Coordinate(0., 1.), Coordinate(-1., 0.)}));
}

TEST_CASE("Polygon Equals") {
    const auto vertices1 = std::vector<Coordinate>{Coordinate(1., 2.), Coordinate(2., 1.), Coordinate(3., 3.)};
    const auto vertices2 = std::vector<Coordinate>{Coordinate(1., 2.), Coordinate(2., 2.), Coordinate(1., 1.)};

    const auto poly1 = Polygon(vertices1);
    const auto poly2 = Polygon(vertices1);
    const auto poly3 = Polygon(vertices2);

    REQUIRE(poly1 == poly2);
    REQUIRE_FALSE(poly1 != poly2);

    REQUIRE_FALSE(poly1 == poly3);
    REQUIRE(poly1 != poly3);
}

TEST_CASE("Polygon hash") {
    const auto vertices1 = std::vector<Coordinate>{Coordinate(1., 2.), Coordinate(2., 1.), Coordinate(3., 3.)};
    const auto vertices2 = std::vector<Coordinate>{Coordinate(1., 2.), Coordinate(2., 2.), Coordinate(1., 1.)};

    const auto poly1 = Polygon(vertices1);
    const auto poly2 = Polygon(vertices1);
    const auto poly3 = Polygon(vertices2);

    const auto poly1_hash = std::hash<Polygon>()(poly1);
    const auto poly2_hash = std::hash<Polygon>()(poly2);
    const auto poly3_hash = std::hash<Polygon>()(poly3);

    REQUIRE(poly1_hash == poly2_hash);
    REQUIRE(poly1_hash != poly3_hash);
}
