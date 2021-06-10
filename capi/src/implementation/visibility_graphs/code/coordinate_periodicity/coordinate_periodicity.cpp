//
// Created by James.Balajan on 18/05/2021.
//

#include <cmath>

#include "constants/constants.hpp"
#include "coordinate_periodicity.hpp"

static constexpr auto NUM_PERIODIC_VERTICES = 3;

std::vector<Coordinate> periodic_coordinates_from_coordinate(const Coordinate &coordinate) {
    return std::vector<Coordinate>{
        coordinate,
        Coordinate(coordinate.get_longitude_microdegrees() + LONGITUDE_PERIOD_MICRODEGREES,
                   coordinate.get_latitude_microdegrees()),
        Coordinate(coordinate.get_longitude_microdegrees() - LONGITUDE_PERIOD_MICRODEGREES,
                   coordinate.get_latitude_microdegrees()),
    };
}

bool is_coordinate_over_meridian(const Coordinate &coordinate) {
    return (coordinate.get_longitude_microdegrees() < MIN_LONGITUDE_MICRODEGREES) ||
           (coordinate.get_longitude_microdegrees() > MAX_LONGITUDE_MICRODEGREES);
}

Coordinate coordinate_from_periodic_coordinate(const Coordinate &periodic_coordinate) {
    auto adjusted_longitude = periodic_coordinate.get_longitude_microdegrees();
    if (adjusted_longitude > MAX_LONGITUDE_MICRODEGREES) {
        adjusted_longitude -= LONGITUDE_PERIOD_MICRODEGREES;
    }
    if (adjusted_longitude < MIN_LONGITUDE_MICRODEGREES) {
        adjusted_longitude += LONGITUDE_PERIOD_MICRODEGREES;
    }

    return Coordinate(adjusted_longitude, periodic_coordinate.get_latitude_microdegrees());
}

std::vector<Polygon> make_polygons_periodic(const std::vector<Polygon> &polygons) {
    if (polygons.empty()) {
        return polygons;
    }

    auto periodic_polygons = std::vector<Polygon>();
    periodic_polygons.reserve(NUM_PERIODIC_VERTICES * polygons.size());

    for (const auto &polygon : polygons) {
        std::vector<std::vector<Coordinate>> periodic_polygon_coordinates(NUM_PERIODIC_VERTICES);
        for (size_t i = 0; i < NUM_PERIODIC_VERTICES; ++i) {
            periodic_polygon_coordinates[i].reserve(polygon.get_vertices().size());
        }

        for (const auto &vertex : polygon.get_vertices()) {
            const auto periodic_vertex_coordinates = periodic_coordinates_from_coordinate(vertex);
            for (size_t i = 0; i < NUM_PERIODIC_VERTICES; ++i) {
                periodic_polygon_coordinates[i].push_back(periodic_vertex_coordinates[i]);
            }
        }

        for (const auto &polygon_vertices : periodic_polygon_coordinates) {
            periodic_polygons.emplace_back(polygon_vertices);
        }
    }

    return periodic_polygons;
}

std::vector<Coordinate> make_coordinates_periodic(const std::vector<Coordinate> &coordinates) {
    auto periodic_coordinates = std::vector<Coordinate>();
    periodic_coordinates.reserve(NUM_PERIODIC_VERTICES * coordinates.size());

    for (const auto &coordinate : coordinates) {
        for (const auto &periodic_coordinate : periodic_coordinates_from_coordinate(coordinate)) {
            periodic_coordinates.push_back(periodic_coordinate);
        }
    }

    return periodic_coordinates;
}

std::vector<std::shared_ptr<LineSegment>> make_segments_periodic(const std::vector<LineSegment> &segments) {
    auto periodic_segments = std::vector<std::shared_ptr<LineSegment>>();
    periodic_segments.reserve(NUM_PERIODIC_VERTICES * segments.size());

    for (const auto &segment : segments) {
        const auto periodic_endpoint_1 = periodic_coordinates_from_coordinate(segment.get_endpoint_1());
        const auto periodic_endpoint_2 = periodic_coordinates_from_coordinate(segment.get_endpoint_2());

        for (int i = 0; i < NUM_PERIODIC_VERTICES; ++i) {
            periodic_segments.push_back(std::make_shared<LineSegment>(periodic_endpoint_1[i], periodic_endpoint_2[i]));
        }
    }

    return periodic_segments;
}
