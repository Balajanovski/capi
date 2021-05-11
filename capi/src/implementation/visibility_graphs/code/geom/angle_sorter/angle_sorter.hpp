//
// Created by James.Balajan on 6/04/2021.
//

#ifndef CAPI_ANGLE_SORTER_HPP
#define CAPI_ANGLE_SORTER_HPP

#include "types/coordinate/coordinate.hpp"
#include <vector>

class AngleSorter {
  public:
    static void sort_counter_clockwise_around_observer(const Coordinate &observer, std::vector<Coordinate> &vertices);
};

#endif // CAPI_ANGLE_SORTER_HPP
