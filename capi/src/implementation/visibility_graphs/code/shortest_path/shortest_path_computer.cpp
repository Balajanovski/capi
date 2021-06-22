//
// Created by James.Balajan on 3/06/2021.
//

#include <algorithm>
#include <queue>
#include <fmt/core.h>
#include "constants/constants.hpp"
#include "coordinate_periodicity/coordinate_periodicity.hpp"
#include <s2/s2earth.h>
#include "shortest_path_computer.hpp"

struct AStarHeapElement {
    Coordinate node;
    double distance_to_source;
    double heuristic_distance_to_destination;
};

ShortestPathComputer::ShortestPathComputer(const Graph &graph) : _graph(graph), _index(graph.get_polygons()) {}

std::vector<Coordinate> ShortestPathComputer::shortest_path(const Coordinate &source,
                                                            const Coordinate &destination) const {
    const auto normalized_source = coordinate_from_periodic_coordinate(source);
    const auto source_is_on_land = _index.is_point_contained(normalized_source);

    if (source_is_on_land) {
        return std::vector<Coordinate>{source, destination};
    }

    const auto normalized_destination = coordinate_from_periodic_coordinate(destination);
    const auto destination_is_on_land = _index.is_point_contained(normalized_destination);

    if (destination_is_on_land) {
        return std::vector<Coordinate>{source, destination};
    }

    const auto intersections = _index.intersect_with_segments(LineSegment(normalized_source, normalized_destination));

    if (intersections.empty()) {
        return std::vector<Coordinate>{source, destination};
    }

    const auto modified_graph = create_modified_graph(normalized_source, normalized_destination, intersections);
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

Graph ShortestPathComputer::create_modified_graph(const Coordinate &source, const Coordinate &destination, const std::vector<LineSegment> &intersections) const {
    if (intersections.empty()) {
        return _graph;
    }

    if (_graph.has_vertex(source) && _graph.has_vertex(destination)) {
        return _graph;
    }

    auto modified_graph = Graph(_graph);
    const auto half_longitude_period = LONGITUDE_PERIOD * 0.5;

    if (!_graph.has_vertex(source)) {
        modified_graph.add_vertex(source);
        const auto visible_points = _index.points_visible_within_distance(source, source.to_s2_point().Angle(destination.to_s2_point()));

        for (const auto &p : visible_points) {
            const auto is_meridian_crossing =
                std::abs(source.get_longitude() - p.get_longitude()) > half_longitude_period;
            modified_graph.add_edge(source, p, is_meridian_crossing);
        }
    }

    if (!_graph.has_vertex(destination)) {
        modified_graph.add_vertex(destination);
        const auto visible_points = _index.points_visible_within_distance(destination, destination.to_s2_point().Angle(source.to_s2_point()));

        for (const auto &p : visible_points) {
            const auto is_meridian_crossing =
                std::abs(destination.get_longitude() - p.get_longitude()) > half_longitude_period;
            modified_graph.add_edge(destination, p, is_meridian_crossing);
        }
    }

    return modified_graph;
}
