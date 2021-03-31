//
// Created by James.Balajan on 31/03/2021.
//

#ifndef CAPI_COORDINATE_HPP
#define CAPI_COORDINATE_HPP

class Coordinate {
public:
    Coordinate(double longitude_, double latitude_);

    double get_latitude() const;
    double get_longitude() const;

    bool operator==(const Coordinate& other) const;
    bool operator!=(const Coordinate& other) const;
private:
    double latitude;
    double longitude;
};

#endif //CAPI_COORDINATE_HPP
