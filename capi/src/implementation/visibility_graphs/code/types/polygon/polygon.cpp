//
// Created by James.Balajan on 31/03/2021.
//

#include <algorithm>
#include <sstream>
#include <utility>
#include <vector>

#include "polygon.hpp"

Polygon::Polygon() = default;

Polygon::Polygon(std::initializer_list<Coordinate> vertices) : _vertices(Polygon::preprocess_vertices(vertices)) {}

Polygon::Polygon(const std::vector<Coordinate> &vertices) : _vertices(Polygon::preprocess_vertices(vertices)) {}

const std::vector<Coordinate> &Polygon::get_vertices() const { return _vertices; }

bool Polygon::operator==(const Polygon &other) const { return (_vertices == other._vertices); }

bool Polygon::operator!=(const Polygon &other) const { return !(*this == other); }

std::vector<Coordinate> Polygon::preprocess_vertices(const std::vector<Coordinate> &vertices) {
    return normalize_vertex_orientation_to_counter_clockwise(remove_collinear_vertices(vertices));
}

std::vector<Coordinate>
Polygon::normalize_vertex_orientation_to_counter_clockwise(const std::vector<Coordinate> &vertices) {
    if (vertices.size() <= 2) {
        return vertices;
    }

    float winding_sum = 0;
    long num_vertices = vertices.size();
#pragma omp simd reduction(+ : winding_sum)
    for (long i = 0; i < num_vertices; ++i) {
        long next_i = (i + 1) % num_vertices;
        winding_sum += (vertices[next_i].get_longitude() - vertices[i].get_longitude()) *
                       (vertices[next_i].get_latitude() + vertices[i].get_latitude());
    }

    if (winding_sum < 0) {
        return vertices;
    } else {
        auto reversed_vertices = std::vector<Coordinate>(vertices);
        std::reverse(reversed_vertices.begin(), reversed_vertices.end());
        return reversed_vertices;
    }
}

std::vector<Coordinate> Polygon::remove_collinear_vertices(const std::vector<Coordinate> &vertices) {
    const long num_vertices = vertices.size();
    auto filtered_vertices = std::vector<Coordinate>();
    filtered_vertices.reserve(num_vertices);

    for (long i = 0; i < num_vertices; ++i) {
        long prev_i = (i - 1) % num_vertices;
        if (prev_i < 0)
            prev_i += num_vertices;
        long next_i = (i + 1) % num_vertices;

        const auto ab = vertices[i] - vertices[prev_i];
        const auto bc = vertices[next_i] - vertices[i];

        if (!ab.parallel(bc) || ab.scalar_multiple_factor(bc).value() < 0) {
            filtered_vertices.push_back(vertices[i]);
        }
    }

    return filtered_vertices;
}

std::string Polygon::to_string_representation() const {
    auto outs = std::stringstream();

    outs << "Polygon (";
    for (size_t i = 0; i < _vertices.size(); ++i) {
        outs << _vertices[i];

        if (i < _vertices.size() - 1) {
            outs << ", ";
        }
    }
    outs << ")";

    return outs.str();
}

std::size_t std::hash<Polygon>::operator()(const Polygon &poly) const {
    size_t seed = poly.get_vertices().size();
    for (const auto &vert : poly.get_vertices()) {
        seed ^= std::hash<Coordinate>()(vert) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

std::ostream &operator<<(std::ostream &outs, const Polygon &poly) { return outs << poly.to_string_representation(); }
