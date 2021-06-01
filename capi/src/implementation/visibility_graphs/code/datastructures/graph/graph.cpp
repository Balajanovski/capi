//
// Created by James.Balajan on 31/03/2021.
//

#include <algorithm>
#include <fmt/core.h>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

#include "constants/constants.hpp"
#include "coordinate_periodicity/coordinate_periodicity.hpp"
#include "graph.hpp"
#include "visgraph/vistree_generator.hpp"

Graph::Graph() = default;

Graph::Graph(std::vector<Polygon> polygons) : _polygons(std::move(polygons)) {
    for (const auto &polygon : _polygons) {
        for (const auto &vertex : polygon.get_vertices()) {
            _index_to_coordinate_mapping.push_back(vertex);
        }
    }

    _coordinate_to_index_mapping.reserve(_index_to_coordinate_mapping.size());
    for (unsigned int i = 0; i < _index_to_coordinate_mapping.size(); ++i) {
        _coordinate_to_index_mapping[_index_to_coordinate_mapping[i]] = i;
    }
}

Graph::Graph(const Graph &other_graph) = default;

void Graph::add_edge(const Coordinate &a, const Coordinate &b, bool meridian_crossing) {
    if (a == b) {
        return;
    }

    add_directed_edge(a, b, meridian_crossing);
    add_directed_edge(b, a, meridian_crossing);
}

void Graph::add_directed_edge(const Coordinate &a, const Coordinate &b, bool meridian_crossing) {
    decltype(_neighbors)::accessor accessor;

    const auto a_index = coordinate_to_index(a);
    const auto b_index = coordinate_to_index(b);

    _neighbors.insert(accessor, a_index);
    accessor->second[b_index] = meridian_crossing && ((accessor->second.find(b_index) == accessor->second.end()) ||
                                                      (accessor->second[b_index]));

    accessor.release();
}

bool Graph::has_edge(const Coordinate &a, const Coordinate &b) const {
    decltype(_neighbors)::const_accessor accessor;

    const auto found_a_in_neighbors = _neighbors.find(accessor, coordinate_to_index(a));

    const auto has_edge_result =
        (found_a_in_neighbors) && (accessor->second.find(coordinate_to_index(b)) != accessor->second.end());

    accessor.release();

    return has_edge_result;
}

bool Graph::has_vertex(const Coordinate &vertex) const {
    return (_coordinate_to_index_mapping.find(vertex) != _coordinate_to_index_mapping.end());
}

bool Graph::is_edge_meridian_crossing(const Coordinate &a, const Coordinate &b) const {
    decltype(_neighbors)::const_accessor accessor;

    const auto found_a_in_neighbors = _neighbors.find(accessor, coordinate_to_index(a));

    const auto is_meridian_crossing = (found_a_in_neighbors) &&
                                      (accessor->second.find(coordinate_to_index(b)) != accessor->second.end()) &&
                                      (accessor->second.at(coordinate_to_index(b)));

    accessor.release();

    return is_meridian_crossing;
}

std::string Graph::to_string_representation() const {
    auto outs = std::stringstream();

    const auto coord_sorter = [](const Coordinate &lhs, const Coordinate &rhs) {
        return std::hash<Coordinate>()(lhs) < std::hash<Coordinate>()(rhs);
    };

    auto first_neighbor_coords = std::vector<Coordinate>();
    for (const auto &neighbors : _neighbors) {
        first_neighbor_coords.push_back(index_to_coordinate(neighbors.first));
    }
    std::sort(first_neighbor_coords.begin(), first_neighbor_coords.end(), coord_sorter);

    outs << "Graph (\n";
    for (const auto &neighbors_first : first_neighbor_coords) {
        outs << fmt::format("\t[({}, {}) [", neighbors_first.get_longitude(), neighbors_first.get_latitude());

        auto sorted_neighbors = std::vector<Coordinate>();
        const auto curr_neighbors = get_neighbors(neighbors_first);
        sorted_neighbors.reserve(curr_neighbors.size());
        for (const auto &neighbor : curr_neighbors) {
            sorted_neighbors.push_back(neighbor);
        }
        std::sort(sorted_neighbors.begin(), sorted_neighbors.end(), coord_sorter);

        for (const auto &neighbor : sorted_neighbors) {
            outs << fmt::format("({}, {}, meridian_span: {}) ", neighbor.get_longitude(), neighbor.get_latitude(),
                                is_edge_meridian_crossing(neighbors_first, neighbor));
        }
        outs << "]]\n";
    }
    outs << ")";

    return outs.str();
}

bool Graph::operator==(const Graph &other) const {
    if (std::unordered_set<Polygon>(_polygons.begin(), _polygons.end()) !=
            std::unordered_set<Polygon>(other._polygons.begin(), other._polygons.end()) ||
        _neighbors.size() != other._neighbors.size()) {
        return false;
    }

    for (const auto &current_vertex_id : _neighbors) {
        const auto current_vertex = index_to_coordinate(current_vertex_id.first);

        decltype(other._neighbors)::const_accessor accessor;
        if (!other.has_vertex(current_vertex)) {
            return false;
        }

        for (const auto &neighbor : get_neighbors(current_vertex)) {
            if (!other.has_edge(current_vertex, neighbor) ||
                (is_edge_meridian_crossing(current_vertex, neighbor) !=
                 other.is_edge_meridian_crossing(current_vertex, neighbor))) {
                return false;
            }
        }

        accessor.release();
    }

    return true;
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
    const auto found_source = has_vertex(source);
    if (!found_source) {
        modified_graph.add_vertex(source);
        for (const auto &visible_vertex :
             VistreeGenerator::get_visible_vertices_from_root(source, periodic_modified_graph_polygons, false)) {
            modified_graph.add_edge(source, visible_vertex.coord, visible_vertex.is_visible_across_meridian);
        }
    }
    accessor.release();

    const auto found_destination = has_vertex(destination);
    if (!found_destination) {
        modified_graph.add_vertex(destination);
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

        for (const auto &neighbor : modified_graph.get_neighbors(top.node)) {
            const auto meridian_spanning = modified_graph.is_edge_meridian_crossing(top.node, neighbor);

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
    const auto found = _neighbors.find(accessor, coordinate_to_index(vert1));
    if (found && accessor->second.find(coordinate_to_index(vert2)) != accessor->second.end()) {
        are_verts_adjacent = true;
    }
    accessor.release();

    return are_verts_adjacent;
}

std::vector<Coordinate> Graph::get_neighbors(const Coordinate &vertex) const {
    decltype(_neighbors)::const_accessor accessor;
    const auto found_neighbors = _neighbors.find(accessor, coordinate_to_index(vertex));

    auto neighbors = std::vector<Coordinate>();
    if (found_neighbors) {
        neighbors.reserve(accessor->second.size());
        for (const auto &neighbor : accessor->second) {
            neighbors.push_back(index_to_coordinate(neighbor.first));
        }
    }

    accessor.release();

    return neighbors;
}

std::vector<Coordinate> Graph::get_vertices() const { return _index_to_coordinate_mapping; }

std::vector<Polygon> Graph::get_polygons() const { return _polygons; }

void Graph::add_vertex(const Coordinate &vertex) {
    _coordinate_to_index_mapping[vertex] = _index_to_coordinate_mapping.size();
    _index_to_coordinate_mapping.push_back(vertex);
}

inline int Graph::coordinate_to_index(Coordinate coordinate) const {
    try {
        return _coordinate_to_index_mapping.at(coordinate);
    } catch (const std::out_of_range &exception) {
        throw std::runtime_error(fmt::format("Coordinate {} not in graph vertices, so an index cannot be fetched",
                                             coordinate.to_string_representation()));
    }
}

inline Coordinate Graph::index_to_coordinate(unsigned int index) const { return _index_to_coordinate_mapping[index]; }

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
