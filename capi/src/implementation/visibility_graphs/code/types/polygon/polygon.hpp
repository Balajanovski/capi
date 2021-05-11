//
// Created by James.Balajan on 31/03/2021.
//

#ifndef CAPI_POLYGON_HPP
#define CAPI_POLYGON_HPP

#include <initializer_list>
#include <string>
#include <vector>

#include "types/coordinate/coordinate.hpp"

class Polygon {
  public:
    Polygon(std::initializer_list<Coordinate> vertices);
    explicit Polygon(const std::vector<Coordinate> &vertices);

    [[nodiscard]] const std::vector<Coordinate> &get_vertices() const;

    bool operator==(const Polygon &other) const;
    bool operator!=(const Polygon &other) const;

    [[nodiscard]] std::string to_string_representation() const;

  private:
    std::vector<Coordinate> _vertices;

    static std::vector<Coordinate> preprocess_vertices(const std::vector<Coordinate> &vertices);
    static std::vector<Coordinate>
    normalize_vertex_orientation_to_counter_clockwise(const std::vector<Coordinate> &vertices);
    static std::vector<Coordinate> remove_collinear_vertices(const std::vector<Coordinate> &vertices);
};

std::ostream &operator<<(std::ostream &outs, const Polygon &poly);

#endif // CAPI_POLYGON_HPP
