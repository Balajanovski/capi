//
// Created by James.Balajan on 3/06/2021.
//

#ifndef CAPI_SHORTEST_PATH_COMPUTER_HPP
#define CAPI_SHORTEST_PATH_COMPUTER_HPP

#include <cmath>
#include <utility>
#include <vector>
#include <memory>

#include "datastructures/i_graph/i_graph.hpp"
#include "datastructures/spatial_segment_index/spatial_segment_index.hpp"
#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"


struct LandCollisionCorrection {
    Coordinate corrected_source;
    Coordinate corrected_dest;
    std::optional<LineSegment> corrected_source_edge;
};

struct BatchInterpolateResult {
    std::optional<std::vector<Coordinate>> path;
    std::optional<std::string> error_msg;
};

class ShortestPathComputer {
  public:
    explicit ShortestPathComputer(const std::shared_ptr<IGraph> &graph);
    [[nodiscard]] std::vector<Coordinate> shortest_path(const Coordinate &source, const Coordinate &destination,
                                                        double maximum_distance_to_search_from_source = INFINITY,
                                                        bool correct_vertices_on_land = false,
                                                        double a_star_greediness_weighting = 1.0) const;
    [[nodiscard]] std::vector<BatchInterpolateResult>
    shortest_paths(const std::vector<std::pair<Coordinate, Coordinate>> &source_dest_pairs,
                   double maximum_distance_to_search_from_source = INFINITY, bool correct_vertices_on_land = false,
                   double a_star_greediness_weighting = 1.0) const;

  private:
    static double distance_measurement(const Coordinate &a, const Coordinate &b, bool is_meridian_spanning);
    static double heuristic_distance_measurement(const Coordinate &a, const Coordinate &b);

    [[nodiscard]] LandCollisionCorrection handle_land_collisions(const Coordinate &source,
                                                                 const Coordinate &destination,
                                                                 bool correct_vertices_on_land) const;
    [[nodiscard]] std::shared_ptr<IGraph> create_modified_graph(const Coordinate &source, const Coordinate &destination) const;

    std::shared_ptr<IGraph> _graph;
    SpatialSegmentIndex _index;
};

#endif // CAPI_SHORTEST_PATH_COMPUTER_HPP
