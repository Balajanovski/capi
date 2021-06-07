//
// Created by James.Balajan on 31/03/2021.
//

#include <cmath>
#include <fmt/format.h>
#include <stdexcept>

#include "coordinate.hpp"

#define DEGREES_TO_MICRODEGREES(x) (static_cast<int32_t>(1e6 * (x)))
#define MICRODEGREES_TO_DEGREES(x) (1e-6 * (x))

Coordinate::Coordinate() : _longitude(0), _latitude(0) {}

Coordinate::Coordinate(double longitude, double latitude)
    : _longitude(DEGREES_TO_MICRODEGREES(longitude)), _latitude(DEGREES_TO_MICRODEGREES(latitude)) {}

Coordinate::Coordinate(int32_t longitude_microdegrees, int32_t latitude_microdegrees)
    : _longitude(longitude_microdegrees), _latitude(latitude_microdegrees) {}

double Coordinate::get_latitude() const { return MICRODEGREES_TO_DEGREES(_latitude); }

double Coordinate::get_longitude() const { return MICRODEGREES_TO_DEGREES(_longitude); }

int32_t Coordinate::get_latitude_microdegrees() const { return _latitude; }

int32_t Coordinate::get_longitude_microdegrees() const { return _longitude; }

int64_t Coordinate::get_latitude_microdegrees_long() const { return _latitude; }

int64_t Coordinate::get_longitude_microdegrees_long() const { return _longitude; }

bool Coordinate::operator==(const Coordinate &other) const {
    return _longitude == other._longitude && _latitude == other._latitude;
}

bool Coordinate::operator!=(const Coordinate &other) const { return !(*this == other); }

Coordinate Coordinate::operator+(const Coordinate &other) const {
    return {_longitude + other._longitude, _latitude + other._latitude};
}

Coordinate Coordinate::operator-() const { return {-_longitude, -_latitude}; }

Coordinate Coordinate::operator-(const Coordinate &other) const { return (*this) + (-other); }

Coordinate Coordinate::operator*(double scalar) const {
    return {static_cast<int32_t>(std::round(_longitude * scalar)),
            static_cast<int32_t>(std::round(_latitude * scalar))};
}

Coordinate Coordinate::operator/(double scalar) const { return (*this) * (1 / scalar); }

double Coordinate::dot_product(const Coordinate &other) const {
    const auto longitude = get_longitude();
    const auto latitude = get_latitude();
    const auto other_longitude = other.get_longitude();
    const auto other_latitude = other.get_latitude();

    return (longitude * other_longitude) + (latitude * other_latitude);
}

int64_t Coordinate::dot_product_microdegrees(const Coordinate& other) const {
    const auto longitude = get_longitude_microdegrees_long();
    const auto latitude = get_latitude_microdegrees_long();
    const auto other_longitude = other.get_longitude_microdegrees_long();
    const auto other_latitude = other.get_latitude_microdegrees_long();

    return (longitude * other_longitude / 1000000) + (latitude * other_latitude / 1000000);
}

double Coordinate::cross_product_magnitude(const Coordinate &other) const {
    const auto longitude = get_longitude();
    const auto latitude = get_latitude();
    const auto other_longitude = other.get_longitude();
    const auto other_latitude = other.get_latitude();

    return ((longitude * other_latitude) - (other_longitude * latitude));
}

int64_t Coordinate::cross_product_magnitude_microdegrees(const Coordinate &other) const {
    return (get_longitude_microdegrees_long() * other.get_latitude_microdegrees_long()) - (other.get_longitude_microdegrees_long() * get_latitude_microdegrees_long());
}

double Coordinate::magnitude_squared() const {
    const auto longitude = get_longitude();
    const auto latitude = get_latitude();

    return ((longitude * longitude) + (latitude * latitude));
}

int64_t Coordinate::magnitude_squared_microdegrees() const {
    const auto longitude = get_longitude_microdegrees_long();
    const auto latitude = get_latitude_microdegrees_long();

    return (longitude * longitude / 1000000) + (latitude * latitude / 1000000);
}

double Coordinate::magnitude() const { return std::sqrt(magnitude_squared()); }

Orientation Coordinate::vector_orientation(const Coordinate &v2) const {
    const auto cross_prod = cross_product_magnitude_microdegrees(v2);

    if (cross_prod < 0) {
        return Orientation::CLOCKWISE;
    } else if (cross_prod > 0) {
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
        return static_cast<double>(other._latitude) / _latitude;
    } else {
        return static_cast<double>(other._longitude) / _longitude;
    }
}

double Coordinate::angle_to_horizontal() const {
    const auto longitude = get_longitude_microdegrees();
    const auto latitude = get_latitude_microdegrees();

    auto arctan = std::atan2(latitude, longitude);
    if (arctan < 0)
        arctan += (2 * M_PI);

    return arctan;
}

std::string Coordinate::to_string_representation() const {
    return fmt::format("({},{})", get_longitude(), get_latitude());
}

std::size_t std::hash<Coordinate>::operator()(const Coordinate &coord) const {
    const auto longitude = coord.get_longitude_microdegrees();
    const auto latitude = coord.get_latitude_microdegrees();

    const std::size_t long_hash = hash<int32_t>()(longitude);
    const std::size_t lat_hash = hash<int32_t>()(latitude);
    return lat_hash ^ (long_hash + 0x9e3779b9 + (lat_hash << 6) + (lat_hash >> 2));
}

std::ostream &operator<<(std::ostream &outs, const Coordinate &coord) {
    return outs << coord.to_string_representation();
}
