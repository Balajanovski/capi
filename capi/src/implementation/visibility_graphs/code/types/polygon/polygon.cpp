//
// Created by James.Balajan on 31/03/2021.
//

#include <utility>
#include <algorithm>

#include "polygon.hpp"

Polygon::Polygon(std::initializer_list<Coordinate> vertices) : _vertices(Polygon::normalize_vertex_orientation_to_counter_clockwise(vertices)) {}

Polygon::Polygon(const std::vector<Coordinate>& vertices) : _vertices(Polygon::normalize_vertex_orientation_to_counter_clockwise(vertices)) {}

const std::vector<Coordinate> &Polygon::get_vertices() const { return _vertices; }

bool Polygon::operator==(const Polygon &other) const { return (_vertices == other._vertices); }

bool Polygon::operator!=(const Polygon &other) const { return !(*this == other); }

std::vector<Coordinate>
Polygon::normalize_vertex_orientation_to_counter_clockwise(const std::vector<Coordinate> &vertices) {
    double winding_sum = 0;
    long num_vertices = vertices.size();
    for (long i = 0; i < num_vertices; ++i) {
        long next_i = (i + 1) % num_vertices;

        winding_sum += (vertices[next_i].get_longitude() - vertices[i].get_longitude()) * (vertices[next_i].get_latitude() + vertices[i].get_latitude());
    }

    if (winding_sum < 0) {
        return vertices;
    } else {
        auto reversed_vertices = std::vector<Coordinate>(vertices);
        std::reverse(reversed_vertices.begin(), reversed_vertices.end());
        return reversed_vertices;
    }
}
