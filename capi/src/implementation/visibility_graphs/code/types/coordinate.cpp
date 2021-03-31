//
// Created by James.Balajan on 31/03/2021.
//

#include "types/coordinate.hpp"


Coordinate::Coordinate(double latitude_, double longitude_): latitude(latitude_), longitude(longitude_) { }


double Coordinate::get_latitude() const {
    return latitude;
}


double Coordinate::get_longitude() const {
    return longitude;
}


bool Coordinate::operator==(const Coordinate &other) const {
    return (latitude == other.latitude) && (longitude == other.longitude);
}


bool Coordinate::operator!=(const Coordinate &other) const {
    return !(*this == other);
}
