//
// Created by James.Balajan on 3/06/2021.
//

#include <algorithm>
#include <queue>
#include <iostream>
#include <fmt/format.h>

#include "coordinate_periodicity/coordinate_periodicity.hpp"
#include "shortest_path_computer.hpp"
#include "datastructures/modified_graph/modified_graph.hpp"

struct AStarHeapElement {
    Coordinate node;
    double distance_to_source;
    double heuristic_distance_to_destination;
};

ShortestPathComputer::ShortestPathComputer(const std::shared_ptr<IGraph> &graph) : _graph(graph), _index(graph->get_polygons()) {}

std::vector<Coordinate> ShortestPathComputer::shortest_path(const Coordinate &source, const Coordinate &destination,
                                                            double maximum_distance_to_search_from_source,
                                                            bool correct_vertices_on_land,
                                                            double a_star_greediness_weighting) const {
    const auto normalized_source = coordinate_from_periodic_coordinate(source);
    const auto normalized_destination = coordinate_from_periodic_coordinate(destination);

    const auto land_corrections =
        handle_land_collisions(normalized_source, normalized_destination, correct_vertices_on_land);
    const auto &corrected_source = land_corrections.corrected_source;
    const auto &corrected_dest = land_corrections.corrected_dest;

    auto intersections = _index.intersect_with_segments(LineSegment(corrected_source, corrected_dest));
    if (land_corrections.corrected_source_edge) {
        intersections.insert(intersections.begin(), land_corrections.corrected_source_edge.value());
    }

    if (intersections.empty()) {
        return std::vector<Coordinate>{corrected_source, corrected_dest};
    }

    const auto modified_graph = create_modified_graph(corrected_source, corrected_dest);
    const auto source_destination_distance = heuristic_distance_measurement(corrected_source, corrected_dest);

    const auto comparison_func = [&](const AStarHeapElement &a, const AStarHeapElement &b) {
        return (a.distance_to_source + a.heuristic_distance_to_destination * a_star_greediness_weighting) >
               (b.distance_to_source + b.heuristic_distance_to_destination * a_star_greediness_weighting);
    };

    auto pq = std::priority_queue<AStarHeapElement, std::vector<AStarHeapElement>, decltype(comparison_func)>(
        comparison_func);
    pq.push(AStarHeapElement{
        .node = corrected_source,
        .distance_to_source = 0,
        .heuristic_distance_to_destination = source_destination_distance,
    });

    auto prev_coord = std::unordered_map<Coordinate, Coordinate>();
    auto distances_to_source = std::unordered_map<Coordinate, double>();
    while (!pq.empty()) {
        const auto top = pq.top();
        pq.pop();

        if (top.node == corrected_dest) {
            break;
        }

        for (const auto &neighbor : modified_graph->get_neighbors(top.node)) {
            const auto meridian_spanning = modified_graph->is_edge_meridian_crossing(top.node, neighbor);

            const auto edge_dist = distance_measurement(neighbor, top.node, meridian_spanning);
            const auto neighbor_dist_to_source = top.distance_to_source + edge_dist;
            const auto neighbor_direct_distance_to_source = heuristic_distance_measurement(corrected_source, neighbor);
            if (edge_dist > source_destination_distance ||
                neighbor_direct_distance_to_source > maximum_distance_to_search_from_source ||
                (distances_to_source.find(neighbor) != distances_to_source.end() &&
                 distances_to_source.at(neighbor) <= neighbor_dist_to_source)) {
                continue;
            }

            const auto heap_elem = AStarHeapElement{
                .node = neighbor,
                .distance_to_source = neighbor_dist_to_source,
                .heuristic_distance_to_destination = heuristic_distance_measurement(neighbor, corrected_dest),
            };
            pq.push(heap_elem);
            prev_coord[neighbor] = top.node;
            distances_to_source[neighbor] = neighbor_dist_to_source;
        }
    }

    auto path = std::vector<Coordinate>();
    auto curr_node = corrected_dest;

    while (curr_node != corrected_source) {
        path.push_back(curr_node);

        if (prev_coord.find(curr_node) == prev_coord.end()) {
            throw std::runtime_error(
                    fmt::format("Could not find a shortest path. "
                                "Source: {}. "
                                "Destination: {}.",
                                source.to_string_representation(),
                                destination.to_string_representation()));
        }
        curr_node = prev_coord.at(curr_node);
    }
    path.push_back(corrected_source);

    std::reverse(path.begin(), path.end());

    return path;
}

std::vector<BatchInterpolateResult>
ShortestPathComputer::shortest_paths(const std::vector<std::pair<Coordinate, Coordinate>> &source_dest_pairs,
                                     double maximum_distance_to_search_from_source, bool correct_vertices_on_land,
                                     double a_star_greediness_weighting) const {
    auto paths = std::vector<BatchInterpolateResult>(source_dest_pairs.size());

#pragma omp parallel for shared(source_dest_pairs, paths, maximum_distance_to_search_from_source, \
                                correct_vertices_on_land, a_star_greediness_weighting) default(none) schedule(dynamic)
    for (size_t i = 0; i < source_dest_pairs.size(); ++i) {
        try {
            paths[i] = BatchInterpolateResult{
                .path = std::make_optional(
                    shortest_path(
                        source_dest_pairs[i].first,
                        source_dest_pairs[i].second,
                        maximum_distance_to_search_from_source,
                        correct_vertices_on_land,
                        a_star_greediness_weighting
                    )
                ),
                .error_msg = std::nullopt
            };
        } catch (const std::exception &e) {
            paths[i] = BatchInterpolateResult{
                .path = std::nullopt,
                .error_msg = std::string(e.what())
            };
        }
    }

    return paths;
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

LandCollisionCorrection ShortestPathComputer::handle_land_collisions(const Coordinate &source,
                                                                     const Coordinate &destination,
                                                                     bool correct_vertices_on_land) const {
    const auto source_is_on_land = _index.is_point_contained(source);
    const auto destination_is_on_land = _index.is_point_contained(destination);

    if (!correct_vertices_on_land && (source_is_on_land || destination_is_on_land)) {
        throw std::runtime_error(
                fmt::format("Either the source or the destination are on land. "
                            "Source: {}. Destination: {}.",
                            source.to_string_representation(),
                            destination.to_string_representation()));
    }

    auto corrected_source = source;
    auto corrected_destination = destination;
    std::optional<LineSegment> corrected_source_edge = std::nullopt;
    if (source_is_on_land) {
        const auto closest_seg = _index.closest_segment_to_point(source);
        corrected_source = closest_seg.project(source);
        corrected_source_edge = std::make_optional(closest_seg);
    }
    if (destination_is_on_land) {
        const auto closest_seg = _index.closest_segment_to_point(destination);
        corrected_destination = closest_seg.project(destination);
    }

    return LandCollisionCorrection{
        .corrected_source = corrected_source,
        .corrected_dest = corrected_destination,
        .corrected_source_edge = corrected_source_edge,
    };
}

std::shared_ptr<IGraph> ShortestPathComputer::create_modified_graph(const Coordinate &source, const Coordinate &destination) const {
    if (_graph->has_vertex(source) && _graph->has_vertex(destination)) {
        return _graph;
    }

    auto modified_graph = std::make_shared<ModifiedGraph>(_graph);
    const Coordinate vertices_to_process[2] = {source, destination};

    const auto source_dest_spherical_distance = source.spherical_distance(destination);
    for (const auto &vertex_to_process : vertices_to_process) {
        if (!_graph->has_vertex(vertex_to_process)) {
            modified_graph->add_vertex(vertex_to_process);

            const auto reachable_vertices =
                    _index.reachable_vertices(vertex_to_process, source_dest_spherical_distance);

            for (const auto &point : reachable_vertices) {
                modified_graph->add_edge(vertex_to_process, point.coord, point.is_visible_across_meridian);
            }
        }
    }

    return modified_graph;
}
