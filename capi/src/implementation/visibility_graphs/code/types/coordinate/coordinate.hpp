//
// Created by James.Balajan on 31/03/2021.
//

#ifndef CAPI_COORDINATE_HPP
#define CAPI_COORDINATE_HPP

#include <ostream>
#include <optional>

#include "types/orientation/orientation.hpp"

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
    Coordinate operator/(double scalar) const;
    [[nodiscard]] double dot_product(const Coordinate& other) const;
    [[nodiscard]] double cross_product_magnitude(const Coordinate& other) const;
    [[nodiscard]] double magnitude_squared() const;
    [[nodiscard]] double magnitude() const;
    [[nodiscard]] Orientation vector_orientation(const Coordinate& v2) const;
    [[nodiscard]] bool parallel(const Coordinate& other) const;
    [[nodiscard]] std::optional<double> scalar_multiple_factor(const Coordinate& other) const;
    [[nodiscard]] Coordinate unit_vector() const;
private:
    double _longitude;
    double _latitude;
};

namespace std {
    template<>
    struct hash<Coordinate> {
        std::size_t operator()(const Coordinate& coord) const;
    };
}

std::ostream& operator<<(std::ostream& outs, const Coordinate& coord);

#endif // CAPI_COORDINATE_HPP
