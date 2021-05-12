//
// Created by James.Balajan on 31/03/2021.
//

#include <algorithm>
#include <fmt/core.h>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "graph.hpp"
#include "visgraph/vistree_generator.hpp"

#define BITS_IN_A_BYTE 8u

Graph::Graph() = default;

Graph::Graph(std::vector<Polygon> polygons) : _polygons(std::move(polygons)) {}

Graph::Graph(const Graph &other_graph) : _neighbors(other_graph._neighbors), _polygons(other_graph._polygons) {}

void Graph::add_edge(const Coordinate &a, const Coordinate &b) {
    add_directed_edge(a, b);
    add_directed_edge(b, a);
}

void Graph::add_directed_edge(const Coordinate &a, const Coordinate &b) {
    decltype(_neighbors)::accessor accessor;

    _neighbors.insert(accessor, a);
    accessor->second.insert(b);
    accessor.release();
}

bool Graph::has_edge(const Coordinate &a, const Coordinate &b) const {
    decltype(_neighbors)::const_accessor accessor;

    const auto found_a_in_neighbors = _neighbors.find(accessor, a);

    const auto has_edge_result = (found_a_in_neighbors) && (accessor->second.find(b) != accessor->second.end());

    accessor.release();

    return has_edge_result;
}

std::string Graph::to_string_representation() const {
    auto outs = std::stringstream();

    outs << "Graph (\n";
    for (const auto &neighbors : _neighbors) {
        outs << fmt::format("\t[({}, {}) [", neighbors.first.get_longitude(), neighbors.first.get_latitude());
        for (const auto &neighbor : neighbors.second) {
            outs << fmt::format("({}, {}) ", neighbor.get_longitude(), neighbor.get_latitude());
        }
        outs << "]]\n";
    }
    outs << ")";

    return outs.str();
}

bool Graph::operator==(const Graph &other) const {
    for (const auto &_neighbor : _neighbors) {
        decltype(other._neighbors)::const_accessor accessor;
        const auto found_matching_neighbor = other._neighbors.find(accessor, _neighbor.first);

        if (!found_matching_neighbor || _neighbor.second != accessor->second) {
            return false;
        }

        accessor.release();
    }

    return _polygons == other._polygons;
}

bool Graph::operator!=(const Graph &other) const { return !(*this == other); }

struct AStarHeapElement {
    Coordinate node;
    double distance_to_source;
    double heuristic_distance_to_destination;
};

std::vector<Coordinate> Graph::shortest_path(const Coordinate &source, const Coordinate &destination) const {
    auto modified_graph = Graph(*this);

    decltype(modified_graph._neighbors)::const_accessor accessor;
    const auto found_source = modified_graph._neighbors.find(accessor, source);
    if (!found_source) {
        for (const auto &visible_vertex :
             VistreeGenerator::get_visible_vertices_from_root(source, modified_graph._polygons, false)) {
            modified_graph.add_edge(source, visible_vertex);
        }
    }
    accessor.release();

    const auto found_destination = modified_graph._neighbors.find(accessor, destination);
    if (!found_destination) {
        for (const auto &visible_vertex :
             VistreeGenerator::get_visible_vertices_from_root(destination, modified_graph._polygons, false)) {
            modified_graph.add_edge(destination, visible_vertex);
        }
    }
    accessor.release();

    const auto distance_measurement = [](const Coordinate &a, const Coordinate &b) { return (a - b).magnitude(); };

    const auto comparison_func = [&](const AStarHeapElement &a, const AStarHeapElement &b) {
        return (a.distance_to_source + a.heuristic_distance_to_destination) >
               (b.distance_to_source + b.heuristic_distance_to_destination);
    };
    auto pq = std::priority_queue<AStarHeapElement, std::vector<AStarHeapElement>, decltype(comparison_func)>(
        comparison_func);
    pq.push(AStarHeapElement{
        .node = source,
        .distance_to_source = 0,
        .heuristic_distance_to_destination = distance_measurement(source, destination),
    });

    auto prev_coord = std::unordered_map<Coordinate, Coordinate>();
    auto distances_to_source = std::unordered_map<Coordinate, double>();
    while (!pq.empty()) {
        const auto top = pq.top();
        pq.pop();

        if (top.node == destination) {
            break;
        }

        decltype(modified_graph._neighbors)::const_accessor neighbor_accessor;
        modified_graph._neighbors.find(neighbor_accessor, top.node);
        for (const auto &neighbor : neighbor_accessor->second) {
            const auto neighbor_dist_to_source = top.distance_to_source + distance_measurement(neighbor, top.node);

            if (distances_to_source.find(neighbor) != distances_to_source.end() &&
                distances_to_source.at(neighbor) <= neighbor_dist_to_source) {
                continue;
            }

            const auto heap_elem = AStarHeapElement{
                .node = neighbor,
                .distance_to_source = neighbor_dist_to_source,
                .heuristic_distance_to_destination = distance_measurement(neighbor, destination),
            };
            pq.push(heap_elem);
            prev_coord[neighbor] = top.node;
            distances_to_source[neighbor] = neighbor_dist_to_source;
        }
        neighbor_accessor.release();
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

bool Graph::are_adjacent(const Coordinate &vert1, const Coordinate &vert2) const {
    auto are_verts_adjacent = false;

    decltype(_neighbors)::const_accessor accessor;
    const auto found = _neighbors.find(accessor, vert1);
    if (found && accessor->second.find(vert2) != accessor->second.end()) {
        are_verts_adjacent = true;
    }
    accessor.release();

    return are_verts_adjacent;
}

std::vector<Coordinate> Graph::get_neighbors(const Coordinate &vertex) const {
    decltype(_neighbors)::const_accessor accessor;
    const auto found_neighbors = _neighbors.find(accessor, vertex);

    auto neighbors = std::vector<Coordinate>();
    if (found_neighbors) {
        neighbors = std::vector<Coordinate>(accessor->second.begin(), accessor->second.end());
    }

    accessor.release();

    return neighbors;
}

std::vector<Coordinate> Graph::get_vertices() const {
    auto vertices = std::vector<Coordinate>();

    for (const auto &polygon : _polygons) {
        for (const auto &vertex : polygon.get_vertices()) {
            vertices.push_back(vertex);
        }
    }

    return vertices;
}

std::vector<Polygon> Graph::get_polygons() const { return _polygons; }

std::ostream &operator<<(std::ostream &outs, const Graph &graph) { return outs << graph.to_string_representation(); }

#undef BITS_IN_A_BYTE
