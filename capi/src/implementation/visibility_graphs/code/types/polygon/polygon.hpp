//
// Created by James.Balajan on 31/03/2021.
//

#ifndef CAPI_POLYGON_HPP
#define CAPI_POLYGON_HPP

#include <initializer_list>
#include <s2/s2loop.h>
#include <string>
#include <vector>

#include "types/coordinate/coordinate.hpp"
#include "types/line_segment/line_segment.hpp"
#include "types/orientation/orientation.hpp"

class Polygon {
  public:
    Polygon();
    Polygon(std::initializer_list<Coordinate> vertices);
    explicit Polygon(const std::vector<Coordinate> &vertices);

    [[nodiscard]] const std::vector<Coordinate> &get_vertices() const;
    [[nodiscard]] std::vector<LineSegment> get_line_segments() const;

    bool operator==(const Polygon &other) const;
    bool operator!=(const Polygon &other) const;

    [[nodiscard]] S2Loop *to_s2_loop() const;

    [[nodiscard]] std::string to_string_representation() const;

  private:
    std::vector<Coordinate> _vertices;

    static std::vector<Coordinate> preprocess_vertices(const std::vector<Coordinate> &vertices);
    static std::vector<Coordinate>
    normalize_vertex_orientation_to_counter_clockwise(const std::vector<Coordinate> &vertices);
    static std::vector<Coordinate> remove_collinear_vertices(const std::vector<Coordinate> &vertices);
};

namespace std {
template <> struct hash<Polygon> { std::size_t operator()(const Polygon &poly) const; };
} // namespace std

std::ostream &operator<<(std::ostream &outs, const Polygon &poly);

Orientation get_orientation_of_vertices(const std::vector<Coordinate> &vertices);

#endif // CAPI_POLYGON_HPP
