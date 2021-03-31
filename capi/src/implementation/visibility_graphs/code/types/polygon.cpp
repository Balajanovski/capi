//
// Created by James.Balajan on 31/03/2021.
//

#include "polygon.hpp"


Polygon::Polygon(std::initializer_list<Coordinate> vertices): vertex_coords(vertices) { }


const std::vector<Coordinate> & Polygon::get_vertices() const {
    return vertex_coords;
}
