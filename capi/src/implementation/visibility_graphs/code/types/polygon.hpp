//
// Created by James.Balajan on 31/03/2021.
//

#ifndef CAPI_POLYGON_HPP
#define CAPI_POLYGON_HPP

#include <initializer_list>
#include <vector>

#include "types/coordinate.hpp"


class Polygon {
public:
    Polygon(std::initializer_list<Coordinate> vertices);

    const std::vector<Coordinate>& get_vertices() const;
private:
    std::vector<Coordinate> vertex_coords;
};


#endif//CAPI_POLYGON_HPP
