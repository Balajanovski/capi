//
// Created by James.Balajan on 18/05/2021.
//

#include <cmath>

#include "constants/constants.hpp"
#include "coordinate_periodicity.hpp"

std::vector<Coordinate> periodic_coordinates_from_coordinate(const Coordinate &coordinate) {
    return std::vector<Coordinate>{
        coordinate,
        Coordinate(coordinate.get_longitude() - LONGITUDE_PERIOD, coordinate.get_latitude()),
        Coordinate(coordinate.get_longitude() + LONGITUDE_PERIOD, coordinate.get_latitude()),
    };
}

bool is_coordinate_over_meridian(const Coordinate &coordinate) {
    return (coordinate.get_longitude() <= (MIN_LONGITUDE - EPSILON_TOLERANCE)) ||
           (coordinate.get_longitude() >= (MAX_LONGITUDE + EPSILON_TOLERANCE));
}

Coordinate coordinate_from_periodic_coordinate(const Coordinate &periodic_coordinate) {
    auto adjusted_longitude = periodic_coordinate.get_longitude();
    if (adjusted_longitude >= MAX_LONGITUDE + EPSILON_TOLERANCE)
        adjusted_longitude -= LONGITUDE_PERIOD;
    if (adjusted_longitude <= MIN_LONGITUDE - EPSILON_TOLERANCE)
        adjusted_longitude += LONGITUDE_PERIOD;

    return Coordinate(adjusted_longitude, periodic_coordinate.get_latitude());
}

std::vector<Polygon> make_polygons_periodic(const std::vector<Polygon> &polygons) {
    if (polygons.empty()) {
        return polygons;
    }

    const auto num_periodic_vertices = periodic_coordinates_from_coordinate(Coordinate(0, 0)).size();
    auto periodic_polygons = std::vector<Polygon>();
    periodic_polygons.reserve(num_periodic_vertices * polygons.size());

    for (const auto &polygon : polygons) {
        std::vector<std::vector<Coordinate>> periodic_polygon_coordinates(num_periodic_vertices);
        for (size_t i = 0; i < num_periodic_vertices; ++i) {
            periodic_polygon_coordinates[i].reserve(polygon.get_vertices().size());
        }

        for (const auto &vertex : polygon.get_vertices()) {
            const auto periodic_vertex_coordinates = periodic_coordinates_from_coordinate(vertex);
            for (size_t i = 0; i < num_periodic_vertices; ++i) {
                periodic_polygon_coordinates[i].push_back(periodic_vertex_coordinates[i]);
            }
        }

        for (const auto &polygon_vertices : periodic_polygon_coordinates) {
            periodic_polygons.emplace_back(polygon_vertices);
        }
    }

    return periodic_polygons;
}
