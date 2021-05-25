//
// Created by James.Balajan on 31/03/2021.
//

#include <cmath>
#include <fmt/format.h>
#include <stdexcept>

#include "constants/constants.hpp"
#include "coordinate.hpp"
#include "geom/fast_arctan/fast_arctan.hpp"

const std::regex Coordinate::coordinate_regex = std::regex("^\\((-?[0-9]+(?:\\.[0-9]+)?),(-?[0-9]+(?:\\.[0-9]+)?)\\)$");

long discretise_double(double val);

Coordinate::Coordinate() : _longitude(0.0), _latitude(0.0) {}

Coordinate::Coordinate(double longitude, double latitude) {
    _longitude = longitude;
    _latitude = latitude;
}

double Coordinate::get_latitude() const { return _latitude; }

double Coordinate::get_longitude() const { return _longitude; }

bool Coordinate::operator==(const Coordinate &other) const {
    return (other - (*this)).magnitude_squared() <= EPSILON_TOLERANCE_SQUARED;
}

bool Coordinate::operator!=(const Coordinate &other) const { return !(*this == other); }

Coordinate Coordinate::operator+(const Coordinate &other) const {
    return {_longitude + other._longitude, _latitude + other._latitude};
}

Coordinate Coordinate::operator-() const { return {-_longitude, -_latitude}; }

Coordinate Coordinate::operator-(const Coordinate &other) const { return (*this) + (-other); }

Coordinate Coordinate::operator*(double scalar) const { return {_longitude * scalar, _latitude * scalar}; }

Coordinate Coordinate::operator/(double scalar) const { return (*this) * (1 / scalar); }

double Coordinate::dot_product(const Coordinate &other) const {
    return _longitude * other._longitude + _latitude * other._latitude;
}

double Coordinate::cross_product_magnitude(const Coordinate &other) const {
    return (_longitude * other._latitude) - (other._longitude * _latitude);
}

double Coordinate::magnitude_squared() const { return (_longitude * _longitude) + (_latitude * _latitude); }

double Coordinate::magnitude() const { return std::sqrt(magnitude_squared()); }

Orientation Coordinate::vector_orientation(const Coordinate &v2) const {
    const auto cross_prod = this->cross_product_magnitude(v2);

    if (cross_prod < -(EPSILON_TOLERANCE_SQUARED)) {
        return Orientation::CLOCKWISE;
    } else if (cross_prod > (EPSILON_TOLERANCE_SQUARED)) {
        return Orientation::COUNTER_CLOCKWISE;
    } else {
        return Orientation::COLLINEAR;
    }
}

bool Coordinate::parallel(const Coordinate &other) const {
    return this->vector_orientation(other) == Orientation::COLLINEAR;
}

std::optional<double> Coordinate::scalar_multiple_factor(const Coordinate &other) const {
    if (!this->parallel(other)) {
        return {};
    }

    if (_longitude == 0 && _latitude == 0) {
        return 0;
    } else if (_longitude == 0) {
        return other._latitude / _latitude;
    } else {
        return other._longitude / _longitude;
    }
}

double Coordinate::angle_to_horizontal() const {
    if (_longitude == 0) {
        if (_latitude < 0) {
            return M_PI_2 * 3;
        }
        return M_PI_2;
    }

    if (_latitude == 0) {
        if (_longitude < 0) {
            return M_PI;
        }
        return 0;
    }

    const auto arctan = fast_arctan(_latitude / _longitude);
    if (_longitude < 0) {
        return M_PI + arctan;
    }
    if (_latitude < 0) {
        return (2 * M_PI) + arctan;
    }
    return arctan;
}

std::string Coordinate::to_string_representation() const { return fmt::format("({},{})", _longitude, _latitude); }

Coordinate Coordinate::parse_from_string(const std::string &str) {
    std::smatch matches;

    if (std::regex_search(str, matches, Coordinate::coordinate_regex)) {
        return Coordinate(std::stod(matches.str(1)), std::stod(matches.str(2)));
    } else {
        throw std::runtime_error(fmt::format("Could not parse a coordinate from {}", str));
    }
}

std::size_t std::hash<Coordinate>::operator()(const Coordinate &coord) const {
    const auto longitude = coord.get_longitude();
    const auto latitude = coord.get_latitude();

    const std::size_t long_hash = hash<long>()(discretise_double(longitude));
    const std::size_t lat_hash = hash<long>()(discretise_double(latitude));
    return lat_hash ^ (long_hash + 0x9e3779b9 + (lat_hash << 6) + (lat_hash >> 2));
}

long discretise_double(double val) { return static_cast<long>(std::round(val / EPSILON_TOLERANCE)); }

std::ostream &operator<<(std::ostream &outs, const Coordinate &coord) {
    return outs << coord.to_string_representation();
}
