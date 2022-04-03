//
// Created by James.Balajan on 31/03/2021.
//
// Implements the algorithm in: http://dav.ee/papers/Visibility_Graph_Algorithm.pdf

#ifndef CAPI_VISGRAPH_GENERATOR_HPP
#define CAPI_VISGRAPH_GENERATOR_HPP

#include <vector>
#include <memory>
#include <iostream>

#include "datastructures/graph/graph.hpp"
#include "types/polygon/polygon.hpp"

class VisgraphGenerator {
  public:
    explicit VisgraphGenerator();

    [[nodiscard]] static std::shared_ptr<Graph> generate(const std::vector<Polygon> &polygons);
    [[nodiscard]] static std::shared_ptr<Graph> generate_with_shuffled_range(const std::vector<Polygon> &polygons, size_t range_start,
                                                            size_t range_end, unsigned int seed);

  private:
    static std::vector<Coordinate> polygon_vertices(const std::vector<Polygon> &polygons);
};

#endif // CAPI_VISGRAPH_GENERATOR_HPP
