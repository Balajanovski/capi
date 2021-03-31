//
// Created by James.Balajan on 31/03/2021.
//
// Implements the algorithm in: https://dl.acm.org/doi/abs/10.1145/73393.73410

#ifndef CAPI_VISGRAPH_GENERATOR_HPP
#define CAPI_VISGRAPH_GENERATOR_HPP

#include <memory>
#include <vector>

#include "datastructures/graph/graph.hpp"
#include "datastructures/rotation_tree/rotation_tree_node.hpp"
#include "types/polygon/polygon.hpp"

class VisgraphGenerator {
  public:
    explicit VisgraphGenerator(double max_longitude = 180.0, double min_latitude = -90.0, double max_latitude = 90.0);

    Graph generate(const std::vector<Polygon> &polygons) const;

  private:
    static void handle(const std::shared_ptr<RotationTreeNode> &p, const std::shared_ptr<RotationTreeNode> &q,
                       Graph &graph);
    static void sort_verticies_by_position(std::vector<Coordinate> &vertices);
    static std::vector<std::shared_ptr<RotationTreeNode>> create_vertex_nodes(const std::vector<Coordinate> &vertices);

    double _max_longitude;
    double _max_latitude;
    double _min_latitude;
};

#endif // CAPI_VISGRAPH_GENERATOR_HPP
