//
// Created by James.Balajan on 7/04/2021.
//

#include <algorithm>
#include <catch.hpp>
#include <vector>

#include "constants/constants.hpp"
#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"
#include "visgraph/vistree_generator.hpp"

const auto coord_sorter = [](const VisibleVertex &lhs, const VisibleVertex &rhs) {
    if (std::abs(lhs.coord.get_longitude() - rhs.coord.get_longitude()) > EPSILON_TOLERANCE) {
        return lhs.coord.get_longitude() < rhs.coord.get_longitude();
    }

    if (std::abs(lhs.coord.get_latitude() - rhs.coord.get_latitude()) > EPSILON_TOLERANCE) {
        return lhs.coord.get_latitude() < rhs.coord.get_latitude();
    }

    return !lhs.is_visible_across_meridian;
};

TEST_CASE("Vistree Generator get visible vertices from root") {
    const auto root_vertex = Coordinate(1.000000, 0.000);

    const auto poly1 = Polygon({
        Coordinate(1, 0),
        Coordinate(0, 1),
        Coordinate(-1, 0),
        Coordinate(-1, -1),
        Coordinate(0, -1),
        Coordinate(0.3, -0.5),
    });

    const auto poly2 = Polygon({
        Coordinate(3, -1),
        Coordinate(2, -2),
        Coordinate(2.9, -3),
        Coordinate(3, -3),
        Coordinate(4, -2),
    });

    auto visible_vertices =
        VistreeGenerator::get_visible_vertices_from_root(root_vertex, std::vector<Polygon>{poly1, poly2});
    auto expected_vertices = std::vector<VisibleVertex>{
        VisibleVertex{.coord = Coordinate(0, -1), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(2, -2), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(3, -1), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(0, 1), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(0.3, -0.5), .is_visible_across_meridian = false},
    };
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}

TEST_CASE("Vistree Generator get visible vertices from root half scan") {
    const auto root_vertex = Coordinate(1.000000, 0.000);

    const auto poly1 = Polygon({
        Coordinate(1, 0),
        Coordinate(0, 1),
        Coordinate(-1, 0),
        Coordinate(-1, -1),
        Coordinate(0, -1),
        Coordinate(0.3, -0.5),
    });

    const auto poly2 = Polygon({
        Coordinate(3, -1),
        Coordinate(2, -2),
        Coordinate(2.9, -3),
        Coordinate(3, -3),
        Coordinate(4, -2),
    });

    auto visible_vertices =
        VistreeGenerator::get_visible_vertices_from_root(root_vertex, std::vector<Polygon>{poly1, poly2}, true);
    auto expected_vertices = std::vector<VisibleVertex>{
        VisibleVertex{.coord = Coordinate(0, 1), .is_visible_across_meridian = false},
    };
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}

TEST_CASE("Vistree Generator get visible vertices from root root on edge") {
    const auto root_vertex = Coordinate(0.5, 0.5);

    const auto poly1 = Polygon({
        Coordinate(1, 0),
        Coordinate(0, 1),
        Coordinate(-1, 0),
        Coordinate(-1, -1),
        Coordinate(0, -1),
        Coordinate(0.3, -0.5),
    });

    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root_vertex, std::vector<Polygon>{poly1});
    auto expected_vertices = std::vector<VisibleVertex>{};
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}

TEST_CASE("Vistree Generator get visible vertices from root isolated") {
    const auto root_vertex = Coordinate(3.000, -3.000);

    const auto poly1 = Polygon({
        Coordinate(1, 0),
        Coordinate(0, 1),
        Coordinate(-1, 0),
        Coordinate(-1, -1),
        Coordinate(0, -1),
        Coordinate(0.3, -0.5),
    });

    const auto poly2 = Polygon({
        Coordinate(3, -1),
        Coordinate(2, -2),
        Coordinate(2.9, -3),
        Coordinate(3, -3),
        Coordinate(4, -2),
    });

    auto visible_vertices =
        VistreeGenerator::get_visible_vertices_from_root(root_vertex, std::vector<Polygon>{poly1, poly2});
    auto expected_vertices = std::vector<VisibleVertex>{
        VisibleVertex{.coord = Coordinate(2.9, -3), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(4, -2), .is_visible_across_meridian = false},
    };
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}

TEST_CASE("Vistree Generator get visible vertices from concave root") {
    const auto root_vertex = Coordinate(0.3, -0.5);

    const auto poly1 = Polygon({
        Coordinate(1, 0),
        Coordinate(0, 1),
        Coordinate(-1, 0),
        Coordinate(-1, -1),
        Coordinate(0, -1),
        Coordinate(0.3, -0.5),
    });

    const auto poly2 = Polygon({
        Coordinate(3, -1),
        Coordinate(2, -2),
        Coordinate(2.9, -3),
        Coordinate(3, -3),
        Coordinate(4, -2),
    });

    auto visible_vertices =
        VistreeGenerator::get_visible_vertices_from_root(root_vertex, std::vector<Polygon>{poly1, poly2});
    auto expected_vertices = std::vector<VisibleVertex>{
        VisibleVertex{.coord = Coordinate(2, -2), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(2.9, -3), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(3, -1), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(0, -1), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(1, 0), .is_visible_across_meridian = false},
    };
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}

TEST_CASE("Vistree Generator get visible vertices from concave root 2") {
    const auto root = Coordinate(0, 0);

    const auto polygon = Polygon({
        Coordinate(1.5, 1),
        Coordinate(-1.5, 1),
        Coordinate(-1.5, -2),
        Coordinate(-1, -1),
        Coordinate(0, 0),
        Coordinate(1, -1),
        Coordinate(1.5, -2),
    });
    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root, std::vector<Polygon>{polygon});
    auto expected_vertices = std::vector<VisibleVertex>{
        VisibleVertex{.coord = Coordinate(-1.5, -2), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(-1, -1), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(1, -1), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(1.5, -2), .is_visible_across_meridian = false},
    };
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}

TEST_CASE("Vistree Generator get visible vertices from concave root 3") {
    const auto root = Coordinate(1, -1);

    const auto polygon = Polygon({
        Coordinate(1.5, 1),
        Coordinate(-1.5, 1),
        Coordinate(-1.5, -2),
        Coordinate(-1, -1),
        Coordinate(0, 0),
        Coordinate(1, -1),
        Coordinate(1.5, -2),
    });
    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root, std::vector<Polygon>{polygon});
    auto expected_vertices = std::vector<VisibleVertex>{
        VisibleVertex{.coord = Coordinate(0, 0), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(1.5, -2), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(-1, -1), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(-1.5, -2), .is_visible_across_meridian = false},
    };
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}

TEST_CASE("Vistree Generator collinear") {
    const auto polygon = Polygon({
        Coordinate(0.0, 0.0),
        Coordinate(2.0, 2.0),
    });
    const auto root = Coordinate(0.0, 0.0);

    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root, std::vector<Polygon>{polygon});
    auto expected_vertices = std::vector<VisibleVertex>{
        VisibleVertex{.coord = Coordinate(2, 2), .is_visible_across_meridian = false},
    };
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}

TEST_CASE("Vistree Generator collinear above") {
    const auto poly1 = Polygon({
        Coordinate(0, 0),
        Coordinate(1, 1),
        Coordinate(2, -1),
        Coordinate(-2, -1),
        Coordinate(-1, 1),
    });
    const auto poly2 = Polygon({
        Coordinate(0, 2),
        Coordinate(1, 3),
        Coordinate(0, 4),
        Coordinate(-1, 3),
    });
    const auto root = Coordinate(0, 0);

    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root, std::vector<Polygon>{poly1, poly2});
    auto expected_vertices =
        std::vector<VisibleVertex>{VisibleVertex{.coord = Coordinate(-1, 3), .is_visible_across_meridian = false},
                                   VisibleVertex{.coord = Coordinate(-1, 1), .is_visible_across_meridian = false},
                                   VisibleVertex{.coord = Coordinate(0, 2), .is_visible_across_meridian = false},
                                   VisibleVertex{.coord = Coordinate(1, 1), .is_visible_across_meridian = false},
                                   VisibleVertex{.coord = Coordinate(1, 3), .is_visible_across_meridian = false}};
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}

TEST_CASE("Vistree generator collinear 2") {
    const auto poly1 = Polygon({
        Coordinate(0, 1),
        Coordinate(1, 0),
        Coordinate(-1, -1),
        Coordinate(0, 0),
    });

    const auto poly2 = Polygon({
        Coordinate(-10, -0.01),
        Coordinate(-20.1, -1),
        Coordinate(-20, 0),
        Coordinate(-20, -0.02),
    });

    const auto poly3 = Polygon({
        Coordinate(-20 - LONGITUDE_PERIOD, -0.38),
        Coordinate(-30 - LONGITUDE_PERIOD, -0.39),
        Coordinate(-25 - LONGITUDE_PERIOD, -5),
    });

    const auto root = Coordinate(0, 0);

    auto visible_vertices =
        VistreeGenerator::get_visible_vertices_from_root(root, std::vector<Polygon>{poly1, poly2, poly3});
    auto expected_vertices = std::vector<VisibleVertex>{
        VisibleVertex{.coord = Coordinate(-20.1, -1), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(-20, 0), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(-10, -0.01), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(-1, -1), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(0, 1), .is_visible_across_meridian = false},
    };
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}

TEST_CASE("Vistree generator collinear 3") {
    const auto poly1 = Polygon({
        Coordinate(0, 1),
        Coordinate(1, 0),
        Coordinate(-1, -1),
        Coordinate(0, 0),
    });

    const auto poly2 = Polygon({
        Coordinate(-10, -0.001),
        Coordinate(-20.1, -1),
        Coordinate(-20, 0),
        Coordinate(-20, -0.002),
    });

    const auto poly3 = Polygon({
        Coordinate(-20 - LONGITUDE_PERIOD, -0.038),
        Coordinate(-30 - LONGITUDE_PERIOD, -0.03905),
        Coordinate(-25 - LONGITUDE_PERIOD, -5),
    });

    const auto root = Coordinate(0, 0);

    auto visible_vertices =
        VistreeGenerator::get_visible_vertices_from_root(root, std::vector<Polygon>{poly1, poly2, poly3});
    auto expected_vertices = std::vector<VisibleVertex>{
        VisibleVertex{.coord = Coordinate(-20.1, -1), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(-20, 0), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(-10, -0.001), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(-1, -1), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(0, 1), .is_visible_across_meridian = false},
    };
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}

TEST_CASE("Vistree generator collinear 4") {
    const auto poly1 = Polygon({
        Coordinate(-1, -0.5),
        Coordinate(-1.5, -1),
        Coordinate(-2, 1),
        Coordinate(-2.5, -2),
        Coordinate(-0.5, -1),
    });

    const auto poly2 = Polygon({
        Coordinate(-4, -2),
        Coordinate(-5, -3.5),
        Coordinate(-3, -2.5),
    });

    const auto root = Coordinate(0, 0);

    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root, std::vector<Polygon>{poly1, poly2});
    auto expected_vertices = std::vector<VisibleVertex>{
        VisibleVertex{.coord = Coordinate(-2, 1), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(-1, -0.5), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(-0.5, -1), .is_visible_across_meridian = false},
    };
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}

TEST_CASE("Vistree generator collinear 5") {
    const auto poly1 = Polygon({
        Coordinate(-1, -0.5),
        Coordinate(-1.5, -0.75),
        Coordinate(-2, 1),
        Coordinate(-2.5, -2),
        Coordinate(-0.5, -1),
    });

    const auto poly2 = Polygon({
        Coordinate(-4, -2),
        Coordinate(-5, -3.5),
        Coordinate(-3, -2.5),
    });

    const auto root = Coordinate(0, 0);

    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root, std::vector<Polygon>{poly1, poly2});
    auto expected_vertices = std::vector<VisibleVertex>{
        VisibleVertex{.coord = Coordinate(-2, 1), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(-1, -0.5), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(-0.5, -1), .is_visible_across_meridian = false},
    };
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}

TEST_CASE("Vistree generator collinear 6") {
    const auto poly1 = Polygon({
        Coordinate(-1, -0.5),
        Coordinate(-1.5, -1),
        Coordinate(-1.25, -0.625),
        Coordinate(-2, 1),
        Coordinate(-2.5, -2),
        Coordinate(-0.5, -1),
    });

    const auto poly2 = Polygon({
        Coordinate(-4, -2),
        Coordinate(-5, -3.5),
        Coordinate(-3, -2.5),
    });

    const auto root = Coordinate(0, 0);

    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root, std::vector<Polygon>{poly1, poly2});
    auto expected_vertices = std::vector<VisibleVertex>{
        VisibleVertex{.coord = Coordinate(-2, 1), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(-1, -0.5), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(-0.5, -1), .is_visible_across_meridian = false},
    };
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}

TEST_CASE("Vistree generator enclosed periodic") {
    const auto poly1 = Polygon({
        Coordinate(1 + LONGITUDE_PERIOD, 0),
        Coordinate(1 + LONGITUDE_PERIOD, 10),
        Coordinate(2 + LONGITUDE_PERIOD, 10),
        Coordinate(2 + LONGITUDE_PERIOD, -1),
        Coordinate(-2 + LONGITUDE_PERIOD, -1),
        Coordinate(-2 + LONGITUDE_PERIOD, 10),
        Coordinate(-1 + LONGITUDE_PERIOD, 10),
        Coordinate(-1 + LONGITUDE_PERIOD, 0),
    });

    const auto poly2 = Polygon({
        Coordinate(3, -2),
        Coordinate(4, -3),
        Coordinate(3, -4),
        Coordinate(2, -3),
    });

    const auto root = Coordinate(360, 1);

    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root, std::vector<Polygon>{poly1, poly2});
    auto expected_vertices = std::vector<VisibleVertex>{
        VisibleVertex{.coord = Coordinate(-1, 10), .is_visible_across_meridian = true},
        VisibleVertex{.coord = Coordinate(-1, 0), .is_visible_across_meridian = true},
        VisibleVertex{.coord = Coordinate(1, 0), .is_visible_across_meridian = true},
        VisibleVertex{.coord = Coordinate(1, 10), .is_visible_across_meridian = true},
    };
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}

TEST_CASE("Vistree generator enclosed periodic 2") {
    const auto poly1 = Polygon({
        Coordinate(1, 0),
        Coordinate(1, 10),
        Coordinate(2, 10),
        Coordinate(2, -1),
        Coordinate(-2, -1),
        Coordinate(-2, 10),
        Coordinate(-1, 10),
        Coordinate(-1, 0),
    });

    const auto poly2 = Polygon({
        Coordinate(363, -2),
        Coordinate(364, -3),
        Coordinate(363, -4),
        Coordinate(362, -3),
    });

    const auto root = Coordinate(0, 1);

    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root, std::vector<Polygon>{poly1, poly2});
    auto expected_vertices = std::vector<VisibleVertex>{
        VisibleVertex{.coord = Coordinate(-1, 10), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(-1, 0), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(1, 0), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(1, 10), .is_visible_across_meridian = false},
    };
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}

TEST_CASE("Vistree generator enclosed periodic 3") {
    const auto poly1 = Polygon({
        Coordinate(1, 0),
        Coordinate(1, 10),
        Coordinate(2, 10),
        Coordinate(2, -1),
        Coordinate(-2, -1),
        Coordinate(-2, 10),
        Coordinate(-1, 10),
        Coordinate(-1, 0),
    });

    const auto poly2 = Polygon({
        Coordinate(-357, -2),
        Coordinate(-356, -3),
        Coordinate(-357, -4),
        Coordinate(-358, -3),
    });

    const auto root = Coordinate(0, 1);

    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root, std::vector<Polygon>{poly1, poly2});
    auto expected_vertices = std::vector<VisibleVertex>{
        VisibleVertex{.coord = Coordinate(-1, 10), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(-1, 0), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(1, 0), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(1, 10), .is_visible_across_meridian = false},
    };
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}

TEST_CASE("Vistree generator boundaries") {
    const auto polygon_1 = Polygon({
        Coordinate(MAX_LONGITUDE, MAX_LATITUDE),
        Coordinate(MAX_LONGITUDE, MIN_LATITUDE),
        Coordinate(MIN_LONGITUDE, MIN_LATITUDE),
        Coordinate(MIN_LONGITUDE, MAX_LATITUDE),
    });
    const auto polygon_2 = Polygon({
        Coordinate(MAX_LONGITUDE + LONGITUDE_PERIOD, MAX_LATITUDE),
        Coordinate(MAX_LONGITUDE + LONGITUDE_PERIOD, MIN_LATITUDE),
        Coordinate(MIN_LONGITUDE + LONGITUDE_PERIOD, MIN_LATITUDE),
        Coordinate(MIN_LONGITUDE + LONGITUDE_PERIOD, MAX_LATITUDE),
    });
    const auto polygon_3 = Polygon({
        Coordinate(MAX_LONGITUDE - LONGITUDE_PERIOD, MAX_LATITUDE),
        Coordinate(MAX_LONGITUDE - LONGITUDE_PERIOD, MIN_LATITUDE),
        Coordinate(MIN_LONGITUDE - LONGITUDE_PERIOD, MIN_LATITUDE),
        Coordinate(MIN_LONGITUDE - LONGITUDE_PERIOD, MAX_LATITUDE),
    });

    const auto root = Coordinate(MIN_LONGITUDE, MAX_LATITUDE);

    auto visible_vertices =
        VistreeGenerator::get_visible_vertices_from_root(root, std::vector<Polygon>{polygon_1, polygon_2, polygon_3});
    auto expected_vertices = std::vector<VisibleVertex>{
        VisibleVertex{.coord = root, .is_visible_across_meridian = true},
        VisibleVertex{.coord = Coordinate(MIN_LONGITUDE, MIN_LATITUDE), .is_visible_across_meridian = false},
        VisibleVertex{.coord = Coordinate(MAX_LONGITUDE, MIN_LATITUDE), .is_visible_across_meridian = true},
        VisibleVertex{.coord = Coordinate(MAX_LONGITUDE, MAX_LATITUDE), .is_visible_across_meridian = true},
        VisibleVertex{.coord = Coordinate(MAX_LONGITUDE, MAX_LATITUDE), .is_visible_across_meridian = true},
        VisibleVertex{.coord = Coordinate(MAX_LONGITUDE, MAX_LATITUDE), .is_visible_across_meridian = false},
    };
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}