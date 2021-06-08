//
// Created by James.Balajan on 3/06/2021.
//

#include "open_edges.hpp"

void OpenEdges::add_edge(int64_t distance, const LineSegment &segment) {
    _edges_sorted_by_distance.emplace(distance, segment);
    _edge_to_distance_mapping[segment] = distance;
}

void OpenEdges::remove_edge(const LineSegment &segment) {
    if (_edge_to_distance_mapping.find(segment) == _edge_to_distance_mapping.end()) {
        return;
    }

    const auto segment_distance = _edge_to_distance_mapping.at(segment);
    _edge_to_distance_mapping.erase(segment);

    const auto equal_iter = _edges_sorted_by_distance.equal_range(segment_distance);
    for (auto iter = equal_iter.first; iter != equal_iter.second; ++iter) {
        if (iter->second == segment) {
            _edges_sorted_by_distance.erase(iter);
            return;
        }
    }
}

LineSegment OpenEdges::closest_edge() const { return _edges_sorted_by_distance.begin()->second; }

bool OpenEdges::empty() const { return _edges_sorted_by_distance.empty(); }
