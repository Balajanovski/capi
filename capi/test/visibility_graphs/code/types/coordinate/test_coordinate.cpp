//
// Created by James.Balajan on 31/03/2021.
//

#include <catch.hpp>
#include <cmath>
#include <vector>

#include "types/coordinate/coordinate.hpp"

TEST_CASE("Coordinate Constructor") {
    const auto coord = Coordinate(1., 2.);
    REQUIRE(coord.get_longitude() == 1);
    REQUIRE(coord.get_latitude() == 2);
}

TEST_CASE("Coordinate Equals") {
    const auto coord1 = Coordinate(1., 2.);
    const auto coord2 = Coordinate(1., 2.);
    const auto coord3 = Coordinate(2., 1.);

    REQUIRE(coord1 == coord2);
    REQUIRE_FALSE(coord1 != coord2);

    REQUIRE_FALSE(coord1 == coord3);
    REQUIRE(coord1 != coord3);
}

TEST_CASE("Coordinate +") {
    const auto coord1 = Coordinate(1., 2.);
    const auto coord2 = Coordinate(3., 4.);

    REQUIRE(coord1 + coord2 == Coordinate(4., 6.));
}

TEST_CASE("Coordinate unary -") {
    const auto coord = Coordinate(1., 2.);

    REQUIRE(-coord == Coordinate(-1., -2.));
}

TEST_CASE("Coordinate binary -") {
    const auto coord1 = Coordinate(1., 2.);
    const auto coord2 = Coordinate(3., 4.);

    REQUIRE(coord1 - coord2 == Coordinate(-2., -2.));
}

TEST_CASE("Coordinate dot product") {
    const auto coord1 = Coordinate(1., 2.);
    const auto coord2 = Coordinate(3., 4.);

    REQUIRE(coord1.dot_product(coord2) == coord2.dot_product(coord1));
    REQUIRE(coord1.dot_product(coord2) == 11);
}

TEST_CASE("Coordinate magnitude of cross product") {
    const auto coord1 = Coordinate(1., 2.);
    const auto coord2 = Coordinate(3., 4.);

    REQUIRE(coord1.cross_product_magnitude(coord2) == -2);
}

TEST_CASE("Coordinate cross product 0 when parallel") {
    const auto coord1 = Coordinate(1., 2.);
    const auto coord2 = Coordinate(2., 4.);

    REQUIRE(coord1.cross_product_magnitude(coord2) == 0);
}

TEST_CASE("Coordinate cross product 0 when parallel 2") {
    const auto coord1 = Coordinate(1., 2.);
    const auto coord2 = Coordinate(-2., -4.);

    REQUIRE(coord1.cross_product_magnitude(coord2) == 0);
}

TEST_CASE("Coordinate Magnitude squared") {
    const auto coord = Coordinate(1., 2.);

    REQUIRE(coord.magnitude_squared() == 5);
}

TEST_CASE("Coordinate Multiply by scalar") {
    const auto coord = Coordinate(1., 2.);

    REQUIRE((coord * 2) == Coordinate(2., 4.));
}

TEST_CASE("Coordinate parallel") {
    const auto coord1 = Coordinate(1., 2.);
    const auto coord2 = Coordinate(2., 4.);
    const auto coord3 = Coordinate(-1., 2.);
    const auto coord4 = Coordinate(-1., -2.);

    REQUIRE(coord1.parallel(coord2));
    REQUIRE(coord1.parallel(coord4));
    REQUIRE_FALSE(coord1.parallel(coord3));
}

TEST_CASE("Coordinate vector orientation") {
    const auto base = Coordinate(1., 1.);
    const auto counter_clockwise = Coordinate(1., 2.);
    const auto clockwise = Coordinate(2., 1.);
    const auto collinear = Coordinate(2., 2.);

    REQUIRE(base.vector_orientation(counter_clockwise) == Orientation::COUNTER_CLOCKWISE);
    REQUIRE(base.vector_orientation(clockwise) == Orientation::CLOCKWISE);
    REQUIRE(base.vector_orientation(collinear) == Orientation::COLLINEAR);
}

TEST_CASE("Coordinate scalar multiple factor") {
    const auto coord1 = Coordinate(1., 2.);
    const auto coord2 = Coordinate(2., 4.);
    const auto coord3 = Coordinate(-1., -2.);
    const auto coord4 = Coordinate(-1., 2.);
    const auto coord5 = Coordinate(1., -2.);
    const auto coord6 = Coordinate(0., 0.);

    REQUIRE(coord1.scalar_multiple_factor(coord2).value() == 2);
    REQUIRE(coord2.scalar_multiple_factor(coord1).value() == 0.5);
    REQUIRE(coord1.scalar_multiple_factor(coord3).value() == -1);
    REQUIRE(coord1.scalar_multiple_factor(coord6).value() == 0);
    REQUIRE(coord6.scalar_multiple_factor(coord1).value() == 0);
    REQUIRE_FALSE(coord1.scalar_multiple_factor(coord4).has_value());
    REQUIRE_FALSE(coord1.scalar_multiple_factor(coord5).has_value());
}

TEST_CASE("Coordinate angle to horizontal") {
    const auto epsilon_tolerance = 0.000001;

    REQUIRE(std::abs(Coordinate(1., 0.).angle_to_horizontal()) < epsilon_tolerance);
    REQUIRE(std::abs(Coordinate(1., 1.).angle_to_horizontal() - M_PI_4) < epsilon_tolerance);
    REQUIRE(std::abs(Coordinate(0., 1.).angle_to_horizontal() - M_PI_2) < epsilon_tolerance);
    REQUIRE(std::abs(Coordinate(-1., 1.).angle_to_horizontal() - (3 * M_PI_4)) < epsilon_tolerance);
    REQUIRE(std::abs(Coordinate(-1., 0.).angle_to_horizontal() - M_PI) < epsilon_tolerance);
    REQUIRE(std::abs(Coordinate(-1., -1.).angle_to_horizontal() - (5 * M_PI_4)) < epsilon_tolerance);
    REQUIRE(std::abs(Coordinate(0., -1.).angle_to_horizontal() - (3 * M_PI_2)) < epsilon_tolerance);
    REQUIRE(std::abs(Coordinate(1., -1.).angle_to_horizontal() - (7 * M_PI_4)) < epsilon_tolerance);
}

TEST_CASE("Coordinate hash") {
    const auto coord1 = Coordinate(1., 2.);
    const auto coord2 = Coordinate(1.01, 2.01);
    const auto coord3 = Coordinate(1., 2.);

    REQUIRE(std::hash<Coordinate>()(coord1) == std::hash<Coordinate>()(coord3));
    REQUIRE(std::hash<Coordinate>()(coord1) != std::hash<Coordinate>()(coord2));
}

TEST_CASE("Coordinate spherical distance") {
    const auto coord1 = Coordinate(0., 0.);
    const auto coord2 = Coordinate(70., 80.);

    REQUIRE(std::abs(coord1.spherical_distance(coord2) - 1.5113701814) < 0.00001);
}
