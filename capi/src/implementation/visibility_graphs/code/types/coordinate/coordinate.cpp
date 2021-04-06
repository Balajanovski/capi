//
// Created by James.Balajan on 31/03/2021.
//

#include <cmath>
#include <fmt/format.h>

#include "coordinate.hpp"

double round_to_epsilon_tolerance(double val);

Coordinate::Coordinate(double longitude, double latitude) : _latitude(latitude), _longitude(longitude) {}

double Coordinate::get_latitude() const { return _latitude; }

double Coordinate::get_longitude() const { return _longitude; }

bool Coordinate::operator==(const Coordinate &other) const {
    return std::abs(_latitude - other._latitude) < EPSILON_TOLERANCE &&
        std::abs(_longitude - other._longitude) < EPSILON_TOLERANCE;
}

bool Coordinate::operator!=(const Coordinate &other) const { return !(*this == other); }

Coordinate Coordinate::operator+(const Coordinate &other) const {
    return {_longitude + other._longitude, _latitude + other._latitude};
}

Coordinate Coordinate::operator-() const {
    return {-_longitude, -_latitude};
}

Coordinate Coordinate::operator-(const Coordinate &other) const {
    return (*this) + (-other);
}

double Coordinate::dot_product(const Coordinate &other) const {
    return _longitude * other._longitude + _latitude * other._latitude;
}

double Coordinate::cross_product_magnitude(const Coordinate &other) const {
    return (_longitude * other._latitude) - (other._longitude * _latitude);
}

double Coordinate::magnitude_squared() const {
    return (_longitude * _longitude) + (_latitude * _latitude);
}

Coordinate Coordinate::operator*(double scalar) const {
    return {_longitude * scalar, _latitude * scalar};
}

Orientation Coordinate::vector_orientation(const Coordinate &v2) const {
    const auto determinant = _latitude * v2._longitude - _longitude * v2._latitude;

    if (determinant < 0) {
        return Orientation::COUNTER_CLOCKWISE;
    } else if (determinant > 0) {
        return Orientation::CLOCKWISE;
    } else {
        return Orientation::COLLINEAR;
    }
}

std::size_t std::hash<Coordinate>::operator()(const Coordinate &coord) const {
    const std::size_t long_hash = hash<double>()(round_to_epsilon_tolerance(coord.get_longitude()));
    const std::size_t lat_hash = hash<double>()(round_to_epsilon_tolerance(coord.get_latitude()));
    return lat_hash ^ (long_hash + 0x9e3779b9 + (lat_hash << 6) + (lat_hash >> 2));
}

double round_to_epsilon_tolerance(double val) {
    return (std::floor(val / EPSILON_TOLERANCE) * EPSILON_TOLERANCE) + 0.5;
}

std::ostream& operator<<(std::ostream& outs, const Coordinate& coord) {
    return outs << fmt::format("Coordinate ({}, {})", coord.get_longitude(), coord.get_latitude());
}
