//
// Created by James.Balajan on 31/03/2021.
//

#include <utility>

#include "polygon.hpp"

Polygon::Polygon(std::initializer_list<Coordinate> vertices) : _vertices(vertices) {}

Polygon::Polygon(std::vector<Coordinate> vertices) : _vertices(std::move(vertices)) {}

const std::vector<Coordinate> &Polygon::get_vertices() const { return _vertices; }

bool Polygon::operator==(const Polygon &other) const { return (_vertices == other._vertices); }

bool Polygon::operator!=(const Polygon &other) const { return !(*this == other); }
