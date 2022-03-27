//
// Created by James.Balajan on 31/03/2021.
//

#include <algorithm>
#include <fmt/core.h>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

#include "coordinate_periodicity/coordinate_periodicity.hpp"
#include "graph.hpp"

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

void Graph::add_edge(const Coordinate &a, const Coordinate &b, bool meridian_crossing) {
    add_directed_edge(a, b, meridian_crossing);
    add_directed_edge(b, a, meridian_crossing);
}

void Graph::add_directed_edge(const Coordinate &a, const Coordinate &b, bool meridian_crossing) {
    if (a == b) {
        return;
    }

    decltype(_neighbors)::accessor accessor;

    const auto a_index = coordinate_to_index(a);
    const auto b_index = coordinate_to_index(b);

    _neighbors.insert(accessor, a_index);
    accessor->second[b_index] = meridian_crossing && ((accessor->second.find(b_index) == accessor->second.end()) ||
                                                      (accessor->second[b_index]));

    accessor.release();
}

void Graph::remove_edge(const Coordinate &a, const Coordinate &b) {
    remove_directed_edge(a, b);
    remove_directed_edge(b, a);
}

void Graph::remove_directed_edge(const Coordinate &a, const Coordinate &b) {
    if (a == b) {
        return;
    }

    decltype(_neighbors)::accessor accessor;

    const auto a_index = coordinate_to_index(a);
    const auto b_index = coordinate_to_index(b);

    const auto found_a = _neighbors.find(accessor, a_index);
    if (found_a) {
        accessor->second.erase(b_index);
    }

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
            accessor.release();
            return false;
        }

        for (const auto &neighbor : get_neighbors(current_vertex)) {
            if (!other.has_edge(current_vertex, neighbor) ||
                (is_edge_meridian_crossing(current_vertex, neighbor) !=
                 other.is_edge_meridian_crossing(current_vertex, neighbor))) {
                accessor.release();
                return false;
            }
        }

        accessor.release();
    }

    return true;
}

bool Graph::operator!=(const Graph &other) const { return !(*this == other); }

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

std::shared_ptr<Graph> merge_graphs(const std::vector<std::shared_ptr<Graph>> &graphs) {
    auto polygons = std::unordered_set<Polygon>();
    for (const auto &graph : graphs) {
        for (const auto &poly : graph->get_polygons()) {
            polygons.insert(poly);
        }
    }

    auto merged_graph = std::make_shared<Graph>(std::vector<Polygon>(polygons.begin(), polygons.end()));

    #pragma omp parallel for shared(graphs, merged_graph) default(none) schedule(static)
    for (size_t i = 0; i < graphs.size(); ++i) { // NOLINT
        const auto graph = graphs[i];
        for (const auto &vert_1 : graph->get_vertices()) {
            for (const auto &vert_2 : graph->get_vertices()) {
                if (graph->has_edge(vert_1, vert_2)) {
                    merged_graph->add_edge(vert_1, vert_2, graph->is_edge_meridian_crossing(vert_1, vert_2));
                }
            }
        }
    }

    return merged_graph;
}

std::ostream &operator<<(std::ostream &outs, const Graph &graph) { return outs << graph.to_string_representation(); }
