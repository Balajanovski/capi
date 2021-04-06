//
// Created by James.Balajan on 31/03/2021.
//

#ifndef CAPI_COORDINATE_HPP
#define CAPI_COORDINATE_HPP

#include <ostream>

#include "types/orientation/orientation.hpp"

namespace {
    constexpr double EPSILON_TOLERANCE = 0.0000001;
}

class Coordinate {
  public:
    Coordinate(double longitude, double latitude);

    // Getters
    [[nodiscard]] double get_latitude() const;
    [[nodiscard]] double get_longitude() const;

    // Comparison operations
    bool operator==(const Coordinate &other) const;
    bool operator!=(const Coordinate &other) const;

    // Vector operations
    Coordinate operator-() const;
    Coordinate operator-(const Coordinate &other) const;
    Coordinate operator+(const Coordinate &other) const;
    Coordinate operator*(double scalar) const;
    [[nodiscard]] double dot_product(const Coordinate& other) const;
    [[nodiscard]] double cross_product_magnitude(const Coordinate& other) const;
    [[nodiscard]] double magnitude_squared() const;
    [[nodiscard]] Orientation vector_orientation(const Coordinate& v2) const;
  private:
    double _latitude;
    double _longitude;
};

namespace std {
    template<>
    struct hash<Coordinate> {
        std::size_t operator()(const Coordinate& coord) const;
    };
}

std::ostream& operator<<(std::ostream& outs, const Coordinate& coord);

#endif // CAPI_COORDINATE_HPP
