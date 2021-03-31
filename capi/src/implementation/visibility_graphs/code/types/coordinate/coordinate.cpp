//
// Created by James.Balajan on 31/03/2021.
//

#include "coordinate.hpp"

Coordinate::Coordinate(double longitude, double latitude) : _latitude(latitude), _longitude(longitude) {}

double Coordinate::get_latitude() const { return _latitude; }

double Coordinate::get_longitude() const { return _longitude; }

bool Coordinate::operator==(const Coordinate &other) const {
    return (_latitude == other._latitude) && (_longitude == other._longitude);
}

bool Coordinate::operator!=(const Coordinate &other) const { return !(*this == other); }
