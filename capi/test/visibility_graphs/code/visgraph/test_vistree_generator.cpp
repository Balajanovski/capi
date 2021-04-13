//
// Created by James.Balajan on 7/04/2021.
//

#include <catch.hpp>
#include <vector>
#include <algorithm>

#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"
#include "visgraph/vistree_generator.hpp"

const auto coord_sorter = [](const Coordinate& lhs, const Coordinate& rhs) {
    if (lhs.get_longitude() != rhs.get_longitude()) {
        return lhs.get_longitude() < rhs.get_longitude();
    }

    return lhs.get_latitude() < rhs.get_longitude();
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

    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root_vertex, std::vector<Polygon> {poly1, poly2});
    auto expected_vertices = std::vector<Coordinate> {
        Coordinate(0, -1),
        Coordinate(2, -2),
        Coordinate(3, -1),
        Coordinate(0, 1),
        Coordinate(0.3, -0.5),
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

    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root_vertex, std::vector<Polygon> {poly1, poly2}, true);
    auto expected_vertices = std::vector<Coordinate> {
            Coordinate(0, 1)
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

    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root_vertex, std::vector<Polygon> {poly1});
    auto expected_vertices = std::vector<Coordinate> {
            Coordinate(0, 1),
            Coordinate(1, 0),
    };
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}

TEST_CASE("Vistree Generator get visible vertices from root precise") {
    const auto root_vertex = Coordinate(0.0001, 0.000);

    const auto poly1 = Polygon({
                                       Coordinate(0.0001, 0),
                                       Coordinate(0, 0.0001),
                                       Coordinate(-0.0001, 0),
                                       Coordinate(-0.0001, -0.0001),
                                       Coordinate(0, -0.0001),
                                       Coordinate(0.00003, -0.00005),
                               });

    const auto poly2 = Polygon({
                                       Coordinate(0.0003, -0.0001),
                                       Coordinate(0.0002, -0.0002),
                                       Coordinate(0.00029, -0.0003),
                                       Coordinate(0.0003, -0.0003),
                                       Coordinate(0.0004, -0.0002),
                               });

    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root_vertex, std::vector<Polygon> {poly1, poly2});
    auto expected_vertices = std::vector<Coordinate> {
            Coordinate(0, -0.0001),
            Coordinate(0.0002, -0.0002),
            Coordinate(0.0003, -0.0001),
            Coordinate(3e-05, -5e-05),
            Coordinate(0, 0.0001),
    };
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

    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root_vertex, std::vector<Polygon> {poly1, poly2});
    auto expected_vertices = std::vector<Coordinate> {
        Coordinate(2.9, -3),
        Coordinate(4, -2),
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

    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root_vertex, std::vector<Polygon> {poly1, poly2});
    auto expected_vertices = std::vector<Coordinate> {
        Coordinate(2, -2),
        Coordinate(2.9, -3),
        Coordinate(3, -1),
        Coordinate(0, -1),
        Coordinate(1, 0),
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
    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root, std::vector<Polygon> {polygon});
    auto expected_vertices = std::vector<Coordinate>{
        Coordinate(-1.5, -2),
        Coordinate(-1, -1),
        Coordinate(1, -1),
        Coordinate(1.5, -2),
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
    auto visible_vertices = VistreeGenerator::get_visible_vertices_from_root(root, std::vector<Polygon> {polygon});
    auto expected_vertices = std::vector<Coordinate>{
        Coordinate(0, 0),
        Coordinate(1.5, -2),
        Coordinate(-1, -1),
        Coordinate(-1.5, -2),
    };
    std::sort(visible_vertices.begin(), visible_vertices.end(), coord_sorter);
    std::sort(expected_vertices.begin(), expected_vertices.end(), coord_sorter);

    REQUIRE(visible_vertices == expected_vertices);
}
