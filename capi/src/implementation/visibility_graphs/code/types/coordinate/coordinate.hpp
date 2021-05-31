//
// Created by James.Balajan on 31/03/2021.
//

#ifndef CAPI_COORDINATE_HPP
#define CAPI_COORDINATE_HPP

#include <optional>
#include <ostream>
#include <regex>
#include <string>

#include "types/orientation/orientation.hpp"

class Coordinate {
  public:
    Coordinate();
    Coordinate(float longitude, float latitude);

    static Coordinate parse_from_string(const std::string &str);

    // Getters
    [[nodiscard]] float get_latitude() const;
    [[nodiscard]] float get_longitude() const;

    // Comparison operations
    bool operator==(const Coordinate &other) const;
    bool operator!=(const Coordinate &other) const;

    // Vector operations
    Coordinate operator-() const;
    Coordinate operator-(const Coordinate &other) const;
    Coordinate operator+(const Coordinate &other) const;
    Coordinate operator*(float scalar) const;
    Coordinate operator/(float scalar) const;
    [[nodiscard]] float dot_product(const Coordinate &other) const;
    [[nodiscard]] float cross_product_magnitude(const Coordinate &other) const;
    [[nodiscard]] float magnitude_squared() const;
    [[nodiscard]] float magnitude() const;
    [[nodiscard]] Orientation vector_orientation(const Coordinate &v2) const;
    [[nodiscard]] bool parallel(const Coordinate &other) const;
    [[nodiscard]] std::optional<float> scalar_multiple_factor(const Coordinate &other) const;
    [[nodiscard]] float angle_to_horizontal() const;
    [[nodiscard]] std::string to_string_representation() const;

  private:
    static const std::regex coordinate_regex;

    float _longitude;
    float _latitude;
};

namespace std {
template <> struct hash<Coordinate> { std::size_t operator()(const Coordinate &coord) const; };
} // namespace std

std::ostream &operator<<(std::ostream &outs, const Coordinate &coord);

#endif // CAPI_COORDINATE_HPP
