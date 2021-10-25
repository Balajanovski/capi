//
// Created by James.Balajan on 3/06/2021.
//

#ifndef CAPI_SHORTEST_PATH_COMPUTER_HPP
#define CAPI_SHORTEST_PATH_COMPUTER_HPP

#include <cmath>
#include <utility>
#include <vector>

#include "datastructures/graph/graph.hpp"
#include "datastructures/spatial_segment_index/spatial_segment_index.hpp"
#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"

struct LandCollisionCorrection {
    Coordinate corrected_source;
    Coordinate corrected_dest;
    std::optional<LineSegment> corrected_source_edge;
};

class ShortestPathComputer {
  public:
    explicit ShortestPathComputer(const Graph &graph);
    [[nodiscard]] std::vector<Coordinate> shortest_path(const Coordinate &source, const Coordinate &destination,
                                                        double maximum_distance_to_search_from_source = INFINITY,
                                                        bool correct_vertices_on_land = false) const;
    [[nodiscard]] std::vector<std::optional<std::vector<Coordinate>>>
    shortest_paths(const std::vector<std::pair<Coordinate, Coordinate>> &source_dest_pairs,
                   double maximum_distance_to_search_from_source = INFINITY,
                   bool correct_vertices_on_land = false) const;

  private:
    static constexpr double GREEDINESS_WEIGHTING = 1.1;

    static double distance_measurement(const Coordinate &a, const Coordinate &b, bool is_meridian_spanning);
    static double heuristic_distance_measurement(const Coordinate &a, const Coordinate &b);

    [[nodiscard]] LandCollisionCorrection handle_land_collisions(const Coordinate &source, const Coordinate &destination, bool correct_vertices_on_land) const;
    [[nodiscard]] Graph create_modified_graph(const Coordinate &source, const Coordinate &destination,
                                              const std::vector<LineSegment> &intersections) const;

    Graph _graph;
    SpatialSegmentIndex _index;
};

#endif // CAPI_SHORTEST_PATH_COMPUTER_HPP
