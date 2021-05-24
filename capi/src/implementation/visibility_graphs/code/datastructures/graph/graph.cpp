//
// Created by James.Balajan on 31/03/2021.
//

#include <algorithm>
#include <fmt/core.h>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#include "constants/constants.hpp"
#include "coordinate_periodicity/coordinate_periodicity.hpp"
#include "graph.hpp"
#include "visgraph/vistree_generator.hpp"

Graph::Graph() = default;

Graph::Graph(std::vector<Polygon> polygons) : _polygons(std::move(polygons)) {}

Graph::Graph(const Graph &other_graph) : _neighbors(other_graph._neighbors), _polygons(other_graph._polygons) {}

void Graph::add_edge(const Coordinate &a, const Coordinate &b, bool meridian_crossing) {
    if (a == b) {
        return;
    }

    add_directed_edge(a, b, meridian_crossing);
    add_directed_edge(b, a, meridian_crossing);
}

void Graph::add_directed_edge(const Coordinate &a, const Coordinate &b, bool meridian_crossing) {
    decltype(_neighbors)::accessor accessor;

    _neighbors.insert(accessor, a);
    accessor->second[b] =
        meridian_crossing && ((accessor->second.find(b) == accessor->second.end()) || (accessor->second[b]));
    accessor.release();
}

bool Graph::has_edge(const Coordinate &a, const Coordinate &b) const {
    decltype(_neighbors)::const_accessor accessor;

    const auto found_a_in_neighbors = _neighbors.find(accessor, a);

    const auto has_edge_result = (found_a_in_neighbors) && (accessor->second.find(b) != accessor->second.end());

    accessor.release();

    return has_edge_result;
}

bool Graph::is_edge_meridian_crossing(const Coordinate &a, const Coordinate &b) const {
    decltype(_neighbors)::const_accessor accessor;

    const auto found_a_in_neighbors = _neighbors.find(accessor, a);

    const auto is_meridian_crossing =
        (found_a_in_neighbors) && (accessor->second.find(b) != accessor->second.end()) && (accessor->second.at(b));

    accessor.release();

    return is_meridian_crossing;
}

std::string Graph::to_string_representation() const {
    auto outs = std::stringstream();

    outs << "Graph (\n";
    for (const auto &neighbors : _neighbors) {
        outs << fmt::format("\t[({}, {}) [", neighbors.first.get_longitude(), neighbors.first.get_latitude());

        auto sorted_neighbors = std::vector<Coordinate>();
        sorted_neighbors.reserve(neighbors.second.size());
        for (const auto &neighbor : neighbors.second) {
            sorted_neighbors.push_back(neighbor.first);
        }
        std::sort(sorted_neighbors.begin(), sorted_neighbors.end(), [](const Coordinate &lhs, const Coordinate &rhs) {
            return std::hash<Coordinate>()(lhs) < std::hash<Coordinate>()(rhs);
        });

        for (const auto &neighbor : sorted_neighbors) {
            outs << fmt::format("({}, {}, meridian_span: {}) ", neighbor.get_longitude(), neighbor.get_latitude(),
                                is_edge_meridian_crossing(neighbors.first, neighbor));
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

    return std::unordered_set<Polygon>(_polygons.begin(), _polygons.end()) ==
           std::unordered_set<Polygon>(other._polygons.begin(), other._polygons.end());
}

bool Graph::operator!=(const Graph &other) const { return !(*this == other); }

struct AStarHeapElement {
    Coordinate node;
    double distance_to_source;
    double heuristic_distance_to_destination;
};

std::vector<Coordinate> Graph::shortest_path(const Coordinate &source, const Coordinate &destination) const {
    auto modified_graph = Graph(*this);
    const auto periodic_modified_graph_polygons = make_polygons_periodic(modified_graph._polygons);

    decltype(modified_graph._neighbors)::const_accessor accessor;
    const auto found_source = modified_graph._neighbors.find(accessor, source);
    if (!found_source) {
        for (const auto &visible_vertex :
             VistreeGenerator::get_visible_vertices_from_root(source, periodic_modified_graph_polygons, false)) {
            modified_graph.add_edge(source, visible_vertex.coord, visible_vertex.is_visible_across_meridian);
        }
    }
    accessor.release();

    const auto found_destination = modified_graph._neighbors.find(accessor, destination);
    if (!found_destination) {
        for (const auto &visible_vertex :
             VistreeGenerator::get_visible_vertices_from_root(destination, periodic_modified_graph_polygons, false)) {
            modified_graph.add_edge(destination, visible_vertex.coord, visible_vertex.is_visible_across_meridian);
        }
    }
    accessor.release();

    const auto distance_measurement = [](const Coordinate &a, const Coordinate &b, bool is_meridian_spanning) {
        if (!is_meridian_spanning) {
            return (a - b).magnitude();
        } else {
            const auto shifted_a_1 = Coordinate(a.get_longitude() + LONGITUDE_PERIOD, a.get_latitude());
            const auto shifted_a_2 = Coordinate(a.get_longitude() - LONGITUDE_PERIOD, a.get_latitude());
            return std::min((shifted_a_1 - b).magnitude(), (shifted_a_2 - b).magnitude());
        }
    };

    const auto comparison_func = [&](const AStarHeapElement &a, const AStarHeapElement &b) {
        return (a.distance_to_source + a.heuristic_distance_to_destination) >
               (b.distance_to_source + b.heuristic_distance_to_destination);
    };
    auto pq = std::priority_queue<AStarHeapElement, std::vector<AStarHeapElement>, decltype(comparison_func)>(
        comparison_func);
    pq.push(AStarHeapElement{
        .node = source,
        .distance_to_source = 0,
        .heuristic_distance_to_destination =
            std::min(distance_measurement(source, destination, false), distance_measurement(source, destination, true)),
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
        for (const auto &neighbor_is_meridian_spanning : neighbor_accessor->second) {
            const auto neighbor = neighbor_is_meridian_spanning.first;
            const auto meridian_spanning = neighbor_is_meridian_spanning.second;

            const auto neighbor_dist_to_source =
                top.distance_to_source + distance_measurement(neighbor, top.node, meridian_spanning);

            if (distances_to_source.find(neighbor) != distances_to_source.end() &&
                distances_to_source.at(neighbor) <= neighbor_dist_to_source) {
                continue;
            }

            const auto heap_elem = AStarHeapElement{
                .node = neighbor,
                .distance_to_source = neighbor_dist_to_source,
                .heuristic_distance_to_destination = std::min(distance_measurement(neighbor, destination, false),
                                                              distance_measurement(neighbor, destination, true)),
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
        neighbors.reserve(accessor->second.size());
        for (const auto &neighbor : accessor->second) {
            neighbors.push_back(neighbor.first);
        }
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

Graph merge_graphs(const std::vector<Graph> &graphs) {
    auto polygons = std::unordered_set<Polygon>();
    for (const auto &graph : graphs) {
        for (const auto &poly : graph.get_polygons()) {
            polygons.insert(poly);
        }
    }

    auto merged_graph = Graph(std::vector<Polygon>(polygons.begin(), polygons.end()));

#pragma omp parallel for shared(graphs, merged_graph) default(none)
    for (size_t i = 0; i < graphs.size(); ++i) { // NOLINT
        const auto graph = graphs[i];
        for (const auto &vert_1 : graph.get_vertices()) {
            for (const auto &vert_2 : graph.get_vertices()) {
                if (graph.are_adjacent(vert_1, vert_2)) {
                    merged_graph.add_edge(vert_1, vert_2, graph.is_edge_meridian_crossing(vert_1, vert_2));
                }
            }
        }
    }

    return merged_graph;
}

std::ostream &operator<<(std::ostream &outs, const Graph &graph) { return outs << graph.to_string_representation(); }
