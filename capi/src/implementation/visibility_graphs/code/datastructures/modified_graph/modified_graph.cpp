//
// Created by James.Balajan on 23-Feb.-2022.
//

#include "modified_graph.hpp"

ModifiedGraph::ModifiedGraph(std::shared_ptr<IGraph> base_graph): _graph(base_graph) { } // NOLINT

void ModifiedGraph::add_edge(const Coordinate &a, const Coordinate &b, bool meridian_crossing) {
    add_directed_edge(a, b, meridian_crossing);
    add_directed_edge(b, a, meridian_crossing);
}

void ModifiedGraph::add_directed_edge(const Coordinate &a, const Coordinate &b, bool meridian_crossing) {
    if (a == b) {
        return;
    }

    if (_neighbors.find(a) == _neighbors.end()) {
        _neighbors[a] = std::unordered_map<Coordinate, NeighborInfo>();
    }

    if (_neighbors[a].find(b) == _neighbors[a].end()) {
        _neighbors[a][b] = NeighborInfo {
            .is_meridian_crossing = meridian_crossing && (!_graph->has_edge(a, b) || _graph->is_edge_meridian_crossing(a, b)),
            .relationship = NeighborRelationship::EDGE_PRESENT
        };
    } else {
        const auto& curr_neighbor = _neighbors[a][b];

        _neighbors[a][b] = NeighborInfo {
            .is_meridian_crossing = meridian_crossing && curr_neighbor.is_meridian_crossing,
            .relationship = NeighborRelationship::EDGE_PRESENT,
        };
    }
}

void ModifiedGraph::remove_edge(const Coordinate &a, const Coordinate &b) {
    remove_directed_edge(a, b);
    remove_directed_edge(b, a);
}

void ModifiedGraph::remove_directed_edge(const Coordinate &a, const Coordinate &b) {
    if (_neighbors.find(a) == _neighbors.end()) {
        _neighbors[a] = std::unordered_map<Coordinate, NeighborInfo>();
    }

    _neighbors[a][b] = NeighborInfo { .is_meridian_crossing = false, .relationship = NeighborRelationship::EDGE_REMOVED };
}

void ModifiedGraph::add_vertex(const Coordinate &vertex) {
    if (!has_vertex(vertex)) {
        _neighbors[vertex] = std::unordered_map<Coordinate, NeighborInfo>();
    }
}

bool ModifiedGraph::has_vertex(const Coordinate &vertex) const {
    return (
        _graph->has_vertex(vertex) ||
        (_neighbors.find(vertex) != _neighbors.end())
    );
}

bool ModifiedGraph::has_edge(const Coordinate &a, const Coordinate &b) const {
    if (_neighbors.find(a) == _neighbors.end() || _neighbors.at(a).find(b) == _neighbors.at(a).end()) {
        return _graph->has_edge(a, b);
    }

    return _neighbors.at(a).at(b).relationship == NeighborRelationship::EDGE_PRESENT;
}

bool ModifiedGraph::is_edge_meridian_crossing(const Coordinate &a, const Coordinate &b) const {
    if (_neighbors.find(a) == _neighbors.end() || _neighbors.at(a).find(b) == _neighbors.at(a).end()) {
        return _graph->is_edge_meridian_crossing(a, b);
    }

    return _neighbors.at(a).at(b).is_meridian_crossing;
}

std::vector<Coordinate> ModifiedGraph::get_neighbors(const Coordinate &vertex) const {
    auto neighbors = _graph->has_vertex(vertex) ? _graph->get_neighbors(vertex) : std::vector<Coordinate>();
    if (_neighbors.find(vertex) == _neighbors.end()) {
        return neighbors;
    }

    neighbors.reserve(_neighbors.at(vertex).size() + neighbors.size());
    for (const auto& neighbor : _neighbors.at(vertex)) {
        if (neighbor.second.relationship == NeighborRelationship::EDGE_PRESENT) {
            neighbors.push_back(neighbor.first);
        } else if (neighbor.second.relationship == NeighborRelationship::EDGE_REMOVED) {
            neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), neighbor.first), neighbors.end());
        }
    }

    return neighbors;
}

std::vector<Coordinate> ModifiedGraph::get_vertices() const {
    auto base_vertices = _graph->get_vertices();
    base_vertices.reserve(base_vertices.size() + _neighbors.size());

    for (const auto& neighbor : _neighbors) {
        base_vertices.push_back(neighbor.first);
    }

    return base_vertices;
}

std::vector<Polygon> ModifiedGraph::get_polygons() const {
    return _graph->get_polygons();
}
