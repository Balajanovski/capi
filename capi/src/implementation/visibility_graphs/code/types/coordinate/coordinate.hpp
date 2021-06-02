//
// Created by James.Balajan on 31/03/2021.
//

#ifndef CAPI_COORDINATE_HPP
#define CAPI_COORDINATE_HPP

#include <optional>
#include <ostream>
#include <string>

#include "types/orientation/orientation.hpp"

class Coordinate {
  public:
    Coordinate();
    Coordinate(double longitude, double latitude);
    Coordinate(int32_t longitude_microdegrees, int32_t latitude_microdegrees);

    // Getters
    [[nodiscard]] double get_latitude() const;
    [[nodiscard]] double get_longitude() const;
    [[nodiscard]] int32_t get_latitude_microdegrees() const;
    [[nodiscard]] int32_t get_longitude_microdegrees() const;

    // Comparison operations
    bool operator==(const Coordinate &other) const;
    bool operator!=(const Coordinate &other) const;

    // Vector operations
    Coordinate operator-() const;
    Coordinate operator-(const Coordinate &other) const;
    Coordinate operator+(const Coordinate &other) const;
    Coordinate operator*(double scalar) const;
    Coordinate operator/(double scalar) const;
    [[nodiscard]] double dot_product(const Coordinate &other) const;
    [[nodiscard]] int64_t dot_product_microdegrees(const Coordinate &other) const;
    [[nodiscard]] double cross_product_magnitude(const Coordinate &other) const;
    [[nodiscard]] double magnitude_squared() const;
    [[nodiscard]] double magnitude() const;
    [[nodiscard]] Orientation vector_orientation(const Coordinate &v2) const;
    [[nodiscard]] bool parallel(const Coordinate &other) const;
    [[nodiscard]] std::optional<double> scalar_multiple_factor(const Coordinate &other) const;
    [[nodiscard]] double angle_to_horizontal() const;
    [[nodiscard]] std::string to_string_representation() const;

  private:
    // Coordinates are stored in microdegrees
    int32_t _longitude;
    int32_t _latitude;
};

namespace std {
template <> struct hash<Coordinate> { std::size_t operator()(const Coordinate &coord) const; };
} // namespace std

std::ostream &operator<<(std::ostream &outs, const Coordinate &coord);

#endif // CAPI_COORDINATE_HPP
