//
// Created by James.Balajan on 3/06/2021.
//

#include <algorithm>
#include <queue>

#include "constants/constants.hpp"
#include "coordinate_periodicity/coordinate_periodicity.hpp"
#include "shortest_path_computer.hpp"

struct AStarHeapElement {
    Coordinate node;
    double distance_to_source;
    double heuristic_distance_to_destination;
};

ShortestPathComputer::ShortestPathComputer(const Graph &graph) : _graph(graph), _index(graph.get_polygons()) {}

std::vector<Coordinate> ShortestPathComputer::shortest_path(const Coordinate &source,
                                                            const Coordinate &destination) const {
    const auto source_is_on_land = _index.is_point_contained(source);
    const auto destination_is_on_land = _index.is_point_contained(destination);

    if ((source_is_on_land || destination_is_on_land) || (!_index.does_segment_intersect_with_segments(LineSegment(source, destination)))) {
        return std::vector<Coordinate>{source, destination};
    }

    const auto modified_graph = create_modified_graph(source, destination);
    const auto source_destination_distance = heuristic_distance_measurement(source, destination);

    const auto comparison_func = [&](const AStarHeapElement &a, const AStarHeapElement &b) {
        return (a.distance_to_source +
                a.heuristic_distance_to_destination * ShortestPathComputer::GREEDINESS_WEIGHTING) >
               (b.distance_to_source +
                b.heuristic_distance_to_destination * ShortestPathComputer::GREEDINESS_WEIGHTING);
    };

    auto pq = std::priority_queue<AStarHeapElement, std::vector<AStarHeapElement>, decltype(comparison_func)>(
        comparison_func);
    pq.push(AStarHeapElement{
        .node = source,
        .distance_to_source = 0,
        .heuristic_distance_to_destination = heuristic_distance_measurement(source, destination),
    });

    auto prev_coord = std::unordered_map<Coordinate, Coordinate>();
    auto distances_to_source = std::unordered_map<Coordinate, double>();
    while (!pq.empty()) {
        const auto top = pq.top();
        pq.pop();

        if (top.node == destination) {
            break;
        }

        for (const auto &neighbor : modified_graph.get_neighbors(top.node)) {
            const auto meridian_spanning = modified_graph.is_edge_meridian_crossing(top.node, neighbor);

            const auto edge_dist = distance_measurement(neighbor, top.node, meridian_spanning);
            const auto neighbor_dist_to_source = top.distance_to_source + edge_dist;
            if (edge_dist > source_destination_distance ||
                (distances_to_source.find(neighbor) != distances_to_source.end() &&
                 distances_to_source.at(neighbor) <= neighbor_dist_to_source)) {
                continue;
            }

            const auto heap_elem = AStarHeapElement{
                .node = neighbor,
                .distance_to_source = neighbor_dist_to_source,
                .heuristic_distance_to_destination = heuristic_distance_measurement(neighbor, destination),
            };
            pq.push(heap_elem);
            prev_coord[neighbor] = top.node;
            distances_to_source[neighbor] = neighbor_dist_to_source;
        }
    }

    auto path = std::vector<Coordinate>();
    auto curr_node = destination;

    while (curr_node != source) {
        path.push_back(curr_node);

        if (prev_coord.find(curr_node) == prev_coord.end()) {
            throw std::runtime_error("Could not find a shortest path");
        }
        curr_node = prev_coord.at(curr_node);
    }
    path.push_back(source);

    std::reverse(path.begin(), path.end());

    return path;
}

double ShortestPathComputer::distance_measurement(const Coordinate &a, const Coordinate &b, bool is_meridian_spanning) {
    if (!is_meridian_spanning) {
        return (a - b).magnitude();
    } else {
        const auto periodic_coordinates = periodic_coordinates_from_coordinate(a);
        return std::min((periodic_coordinates[1] - b).magnitude(), (periodic_coordinates[2] - b).magnitude());
    }
}

double ShortestPathComputer::heuristic_distance_measurement(const Coordinate &a, const Coordinate &b) {
    return std::min(distance_measurement(a, b, false), distance_measurement(a, b, true));
}

Graph ShortestPathComputer::create_modified_graph(const Coordinate &source, const Coordinate &destination) const {
    const auto normalized_source = coordinate_from_periodic_coordinate(source);
    const auto normalized_destination = coordinate_from_periodic_coordinate(destination);

    if (_graph.has_vertex(normalized_source) && _graph.has_vertex(normalized_destination)) {
        return _graph;
    }

    const auto intersected_edges = _index.intersect_with_segments(LineSegment(normalized_source, normalized_destination));

    if (intersected_edges.empty()) {
        // TODO what we want is a graph with just a single segment
        // that way no need to special case further
        // A* will trivially return shortest path
        return _graph;
    }

    auto modified_graph = Graph(_graph);

    if (!_graph.has_vertex(normalized_source)) {
        modified_graph.add_vertex(normalized_source);
        const auto closest_edge = intersected_edges.front();

        const auto p1 = closest_edge.get_endpoint_1();
        const auto p2 = closest_edge.get_endpoint_2();

        const auto is_meridian_crossing_1 = std::abs(normalized_source.get_longitude() - p1.get_longitude()) > (LONGITUDE_PERIOD * 0.5);
        const auto is_meridian_crossing_2 = std::abs(normalized_source.get_longitude() - p2.get_longitude()) > (LONGITUDE_PERIOD * 0.5);

        modified_graph.add_edge(normalized_source, p1, is_meridian_crossing_1);
        modified_graph.add_edge(normalized_source, p2, is_meridian_crossing_2);
    }

    if (!_graph.has_vertex(normalized_destination)) {
        modified_graph.add_vertex(normalized_destination);
        const auto closest_edge = intersected_edges.back();

        const auto p1 = closest_edge.get_endpoint_1();
        const auto p2 = closest_edge.get_endpoint_2();

        const auto is_meridian_crossing_1 = std::abs(normalized_destination.get_longitude() - p1.get_longitude()) > (LONGITUDE_PERIOD * 0.5);
        const auto is_meridian_crossing_2 = std::abs(normalized_destination.get_longitude() - p2.get_longitude()) > (LONGITUDE_PERIOD * 0.5);

        modified_graph.add_edge(normalized_destination, p1, is_meridian_crossing_1);
        modified_graph.add_edge(normalized_destination, p2, is_meridian_crossing_2);
    }

    return modified_graph;
}
