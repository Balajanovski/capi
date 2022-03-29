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
#include <memory>

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

    _num_coords = _index_to_coordinate_mapping.size();
    _neighbors.resize(_num_coords * _num_coords, Graph::EdgeState::DISCONNECTED);
    _accessor_locks.reserve(_num_coords);
    for (size_t i = 0; i < _num_coords; ++i) {
        _accessor_locks.push_back(std::make_unique<std::shared_mutex>());
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

    const auto a_index = coordinate_to_index(a);
    const auto b_index = coordinate_to_index(b);

    auto &a_mutex = _accessor_locks[a_index];
    auto &b_mutex = _accessor_locks[b_index];

    std::unique_lock<std::shared_mutex> a_lock(*a_mutex, std::defer_lock);
    std::unique_lock<std::shared_mutex> b_lock(*b_mutex, std::defer_lock);

    // Lock order prevents deadlock
    if (a_index < b_index) {
        a_lock.lock();
        b_lock.lock();
    } else {
        b_lock.lock();
        a_lock.lock();
    }

    size_t index = a_index + (_num_coords * b_index);
    _neighbors[index] = static_cast<EdgeState>(
            _neighbors[index] |
            ((meridian_crossing) ? EdgeState::CONNECTED_OVER_MERIDIAN : EdgeState::CONNECTED));
}

void Graph::remove_edge(const Coordinate &a, const Coordinate &b) {
    remove_directed_edge(a, b);
    remove_directed_edge(b, a);
}

void Graph::remove_directed_edge(const Coordinate &a, const Coordinate &b) {
    if (a == b) {
        return;
    }

    const auto a_index = coordinate_to_index(a);
    const auto b_index = coordinate_to_index(b);

    auto &a_mutex = _accessor_locks[a_index];
    auto &b_mutex = _accessor_locks[b_index];

    std::unique_lock<std::shared_mutex> a_lock(*a_mutex, std::defer_lock);
    std::unique_lock<std::shared_mutex> b_lock(*b_mutex, std::defer_lock);

    // Lock order prevents deadlock
    if (a_index < b_index) {
        a_lock.lock();
        b_lock.lock();
    } else {
        b_lock.lock();
        a_lock.lock();
    }

    size_t index = a_index + (_num_coords * b_index);
    _neighbors[index] = EdgeState::DISCONNECTED;
}

bool Graph::has_edge(const Coordinate &a, const Coordinate &b) const {
    if (a == b) {
        return false;
    }

    const auto a_index = coordinate_to_index(a);
    const auto b_index = coordinate_to_index(b);

    auto &a_mutex = _accessor_locks[a_index];
    auto &b_mutex = _accessor_locks[b_index];

    std::shared_lock<std::shared_mutex> a_lock(*a_mutex, std::defer_lock);
    std::shared_lock<std::shared_mutex> b_lock(*b_mutex, std::defer_lock);

    // Lock order prevents deadlock
    if (a_index < b_index) {
        a_lock.lock();
        b_lock.lock();
    } else {
        b_lock.lock();
        a_lock.lock();
    }

    size_t index = a_index + (_num_coords * b_index);
    return _neighbors[index] != EdgeState::DISCONNECTED;
}

bool Graph::has_vertex(const Coordinate &vertex) const {
    return (_coordinate_to_index_mapping.find(vertex) != _coordinate_to_index_mapping.end());
}

bool Graph::is_edge_meridian_crossing(const Coordinate &a, const Coordinate &b) const {
    if (a == b) {
        return false;
    }

    const auto a_index = coordinate_to_index(a);
    const auto b_index = coordinate_to_index(b);

    auto &a_mutex = _accessor_locks[a_index];
    auto &b_mutex = _accessor_locks[b_index];

    std::shared_lock<std::shared_mutex> a_lock(*a_mutex, std::defer_lock);
    std::shared_lock<std::shared_mutex> b_lock(*b_mutex, std::defer_lock);

    // Lock order prevents deadlock
    if (a_index < b_index) {
        a_lock.lock();
        b_lock.lock();
    } else {
        b_lock.lock();
        a_lock.lock();
    }

    size_t index = a_index + (_num_coords * b_index);
    return _neighbors[index] == EdgeState::CONNECTED_OVER_MERIDIAN;
}

std::string Graph::to_string_representation() const {
    auto outs = std::stringstream();

    const auto coord_sorter = [](const Coordinate &lhs, const Coordinate &rhs) {
        return std::hash<Coordinate>()(lhs) < std::hash<Coordinate>()(rhs);
    };

    auto vertices = get_vertices();
    std::sort(vertices.begin(), vertices.end(), coord_sorter);

    outs << "Graph (\n";
    for (const auto &vertex : vertices) {
        outs << fmt::format("\t[({}, {}) [", vertex.get_longitude(), vertex.get_latitude());

        auto neighbors = get_neighbors(vertex);
        std::sort(neighbors.begin(), neighbors.end(), coord_sorter);

        for (const auto &neighbor : neighbors) {
            outs << fmt::format("({}, {}, meridian_span: {}) ", neighbor.get_longitude(), neighbor.get_latitude(),
                                is_edge_meridian_crossing(vertex, neighbor));
        }
        outs << "]]\n";
    }
    outs << ")";

    return outs.str();
}

bool Graph::operator==(const Graph &other) const {
    return to_string_representation() == other.to_string_representation();
}

bool Graph::operator!=(const Graph &other) const { return !(*this == other); }

std::vector<Coordinate> Graph::get_neighbors(const Coordinate &vertex) const {
    const auto index = coordinate_to_index(vertex);
    auto &mutex = _accessor_locks[index];
    std::shared_lock<std::shared_mutex> a_lock(*mutex);

    std::vector<Coordinate> neighbors;
    for (size_t i_offset = 0; i_offset < _num_coords; ++i_offset) {
        size_t i = i_offset + (index * _num_coords);
        if (_neighbors[i] != EdgeState::DISCONNECTED) {
            neighbors.push_back(index_to_coordinate(i_offset));
        }
    }

    return neighbors;
}

std::vector<Coordinate> Graph::get_vertices() const { return _index_to_coordinate_mapping; }

std::vector<Polygon> Graph::get_polygons() const { return _polygons; }

void Graph::add_vertex(const Coordinate &vertex) {
    _coordinate_to_index_mapping[vertex] = _num_coords;
    _index_to_coordinate_mapping.push_back(vertex);

    ++_num_coords;
    _neighbors.resize(_num_coords*_num_coords, EdgeState::DISCONNECTED);
    _accessor_locks.push_back(std::make_unique<std::shared_mutex>());
}

inline unsigned int Graph::coordinate_to_index(Coordinate coordinate) const {
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
