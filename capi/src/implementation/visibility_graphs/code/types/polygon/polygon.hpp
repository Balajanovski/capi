//
// Created by James.Balajan on 31/03/2021.
//

#ifndef CAPI_POLYGON_HPP
#define CAPI_POLYGON_HPP

#include <initializer_list>
#include <vector>

#include "types/coordinate/coordinate.hpp"


class Polygon {
public:
    Polygon(std::initializer_list<Coordinate> vertices);
    explicit Polygon(std::vector<Coordinate> vertices);

    const std::vector<Coordinate>& get_vertices() const;

    bool operator==(const Polygon& other) const;
    bool operator!=(const Polygon& other) const;
private:
    std::vector<Coordinate> _vertices;
};


#endif//CAPI_POLYGON_HPP
