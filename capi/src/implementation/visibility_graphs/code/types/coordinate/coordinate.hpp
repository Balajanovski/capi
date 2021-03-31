//
// Created by James.Balajan on 31/03/2021.
//

#ifndef CAPI_COORDINATE_HPP
#define CAPI_COORDINATE_HPP

class Coordinate {
public:
    Coordinate(double longitude, double latitude);

    double get_latitude() const;
    double get_longitude() const;

    bool operator==(const Coordinate& other) const;
    bool operator!=(const Coordinate& other) const;
private:
    double _latitude;
    double _longitude;
};

#endif //CAPI_COORDINATE_HPP
