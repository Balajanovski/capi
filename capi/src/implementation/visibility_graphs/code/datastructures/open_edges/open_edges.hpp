//
// Created by James.Balajan on 3/06/2021.
//

#ifndef CAPI_OPEN_EDGES_HPP
#define CAPI_OPEN_EDGES_HPP

#include <map>
#include <memory>
#include <unordered_map>

#include "types/line_segment/line_segment.hpp"

class OpenEdges {
  public:
    void add_edge(int64_t distance, const LineSegment &segment);
    void remove_edge(const LineSegment &segment);

    [[nodiscard]] LineSegment closest_edge() const;
    [[nodiscard]] bool empty() const;

  private:
    std::map<int64_t, const LineSegment> _edges_sorted_by_distance;
    std::unordered_map<LineSegment, int64_t> _edge_to_distance_mapping;
};

#endif // CAPI_OPEN_EDGES_HPP
