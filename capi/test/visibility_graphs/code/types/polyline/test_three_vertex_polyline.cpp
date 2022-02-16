//
// Created by James.Balajan on 13/04/2021.
//

#include <catch.hpp>
#include <vector>

#include "types/coordinate/coordinate.hpp"
#include "types/polyline/three_vertex_polyline.hpp"

TEST_CASE("Polyline point visible is visible") {
    const auto polyline = ThreeVertexPolyline(Coordinate(1, 0), Coordinate(2, 0), Coordinate(3, 1));

    REQUIRE(polyline.point_visible(Coordinate(3, -1)));
    REQUIRE(polyline.point_visible(Coordinate(3, 0)));
}

TEST_CASE("Polyline point visible is not visible") {
    const auto polyline = ThreeVertexPolyline(Coordinate(1, 0), Coordinate(2, 0), Coordinate(3, -1));

    REQUIRE_FALSE(polyline.point_visible(Coordinate(3, 1)));
    REQUIRE_FALSE(polyline.point_visible(Coordinate(3, 0)));
}

TEST_CASE("Polyline point visible is visible 2") {
    const auto polyline = ThreeVertexPolyline(Coordinate(0.3, -0.5), Coordinate(1., 0.), Coordinate(0., 1.));

    REQUIRE(polyline.point_visible(Coordinate(2., -2.)));
}

TEST_CASE("Polyline reflex") {
    const auto polyline = ThreeVertexPolyline(Coordinate(-1., 0.), Coordinate(0., 1.), Coordinate(1., 0.));

    REQUIRE(polyline.point_visible(Coordinate(0., 0.)));
    REQUIRE_FALSE(polyline.point_visible(Coordinate(0., 2.)));
}

TEST_CASE("Polyline reflex 2") {
    const auto polyline = ThreeVertexPolyline(Coordinate(-1., -1.), Coordinate(0., 0.), Coordinate(1., -1.));

    REQUIRE(polyline.point_visible(Coordinate(0., -1.)));
    REQUIRE(polyline.point_visible(Coordinate(-1., -1.)));
    REQUIRE(polyline.point_visible(Coordinate(1., -1.)));
    REQUIRE_FALSE(polyline.point_visible(Coordinate(-1.5, 1.)));
    REQUIRE_FALSE(polyline.point_visible(Coordinate(1.5, 1.)));
    REQUIRE_FALSE(polyline.point_visible(Coordinate(0., 1.)));
}

TEST_CASE("Polyline concave 1") {
    const auto polyline = ThreeVertexPolyline(Coordinate(2.9, -3.), Coordinate(3., -3.), Coordinate(4., -2.));

    REQUIRE(polyline.point_visible(Coordinate(2.9, -3.)));
    REQUIRE(polyline.point_visible(Coordinate(4., -2.)));
    REQUIRE_FALSE(polyline.point_visible(Coordinate(0., -1.)));
    REQUIRE_FALSE(polyline.point_visible(Coordinate(1., 0.)));
    REQUIRE_FALSE(polyline.point_visible(Coordinate(0.3, -0.5)));
}

TEST_CASE("Polyline broken case 1") {
    const auto polyline = ThreeVertexPolyline(
        Coordinate(-0.514999,44.9903),
        Coordinate(-0.514999,44.809999),
        Coordinate(-0.56505, 44.85002)
    );

    REQUIRE_FALSE(polyline.point_visible(Coordinate(-0.514999, 38.201167)));
    REQUIRE(polyline.point_visible(Coordinate(-0.544, 44.892)));
}

TEST_CASE("Polyline broken case 2") {
    const auto polyline = ThreeVertexPolyline(
        Coordinate(0.3, -0.5),
        Coordinate(1., 0.),
        Coordinate(0., 1.)
    );

    REQUIRE(polyline.point_visible(Coordinate(0., -1.)));
}

TEST_CASE("Polyline parallel case non-reflex 1") {
    const auto polyline = ThreeVertexPolyline(
        Coordinate(-1., 0.),
        Coordinate(0., 0.),
        Coordinate(0., 1.)
    );

    REQUIRE(polyline.point_visible(Coordinate(0., 1.)));
    REQUIRE(polyline.point_visible(Coordinate(0., 0.5)));
    REQUIRE(polyline.point_visible(Coordinate(0., -2.)));
    REQUIRE_FALSE(polyline.point_visible(Coordinate(0., 2.)));
    REQUIRE_FALSE(polyline.point_visible(Coordinate(-1., 1.)));
}

TEST_CASE("Polyline parallel case non-reflex 2") {
    const auto polyline = ThreeVertexPolyline(
            Coordinate(-1., 0.),
            Coordinate(1., 0.),
            Coordinate(0., 1.)
    );

    REQUIRE(polyline.point_visible(Coordinate(-1., 0.)));
    REQUIRE(polyline.point_visible(Coordinate(3., 0.)));
    REQUIRE(polyline.point_visible(Coordinate(5., 0.)));
}

TEST_CASE("Polyline parallel case reflex") {
    const auto polyline = ThreeVertexPolyline(
        Coordinate(0., 1.),
        Coordinate(0., 0.),
        Coordinate(-1., 0.)
    );

    REQUIRE(polyline.point_visible(Coordinate(0., 1.)));
    REQUIRE(polyline.point_visible(Coordinate(0., 0.5)));
    REQUIRE_FALSE(polyline.point_visible(Coordinate(0., -2.)));
    REQUIRE_FALSE(polyline.point_visible(Coordinate(0., 2.)));
    REQUIRE(polyline.point_visible(Coordinate(-1., 1.)));
}
