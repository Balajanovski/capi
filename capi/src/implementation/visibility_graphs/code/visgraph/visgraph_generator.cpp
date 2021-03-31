//
// Created by James.Balajan on 31/03/2021.
//

#include <algorithm>
#include <stack>

#include "geom/left_turn/left_turn.hpp"
#include "visgraph_generator.hpp"

VisgraphGenerator::VisgraphGenerator(double max_longitude, double min_latitude, double max_latitude)
    : _max_latitude(max_latitude), _max_longitude(max_longitude), _min_latitude(min_latitude) {}

Graph VisgraphGenerator::generate(const std::vector<Polygon> &polygons) const {
    auto vertices = std::vector<Coordinate>();
    for (const auto &polygon : polygons) {
        const auto &poly_vertices = polygon.get_vertices();
        vertices.reserve(vertices.size() + poly_vertices.size());
        vertices.insert(vertices.end(), poly_vertices.begin(), poly_vertices.end());
    }

    if (vertices.empty()) {
        return Graph(0);
    }

    sort_verticies_by_position(vertices);

    auto p_infinity = std::make_shared<RotationTreeNode>(Coordinate(_max_longitude, _max_latitude));
    auto p_negative_infinity = std::make_shared<RotationTreeNode>(Coordinate(_max_longitude, _min_latitude));
    auto vertex_nodes = create_vertex_nodes(vertices);

    p_infinity->add_node_as_rightmost_son(p_negative_infinity);
    for (const auto &vertex_node : vertex_nodes) {
        p_negative_infinity->add_node_as_rightmost_son(vertex_node);
    }

    auto visibility_graph = Graph(vertices.size());
    auto leftmost_son_of_father_stack = std::stack<std::shared_ptr<RotationTreeNode>>();
    leftmost_son_of_father_stack.push(vertex_nodes[0]);

    while (!leftmost_son_of_father_stack.empty()) {
        auto p = leftmost_son_of_father_stack.top();
        leftmost_son_of_father_stack.pop();
        auto p_right_brother = p->get_right_brother();
        auto q = p->get_father();

        if (q != p_negative_infinity) {
            handle(p, q, visibility_graph);
        }

        auto z = q->get_left_brother();
        p->detach_from_tree();

        if (z == nullptr || !left_turn(p->get_coordinate(), z->get_coordinate(), z->get_father()->get_coordinate())) {
            q->add_node_as_left_brother(p);
        } else {
            while (z->get_rightmost_son() != nullptr &&
                   left_turn(p->get_coordinate(), z->get_rightmost_son()->get_coordinate(), z->get_coordinate())) {
                z = z->get_rightmost_son();
            }
            z->add_node_as_rightmost_son(p);

            if (z == leftmost_son_of_father_stack.top()) {
                leftmost_son_of_father_stack.pop();
            }
        }

        if (p->get_left_brother() == nullptr && p->get_father() == p_infinity) {
            leftmost_son_of_father_stack.push(p);
        }
        if (p_right_brother != nullptr) {
            leftmost_son_of_father_stack.push(p_right_brother);
        }
    }

    return visibility_graph;
}

void VisgraphGenerator::handle(const std::shared_ptr<RotationTreeNode> &p, const std::shared_ptr<RotationTreeNode> &q,
                               Graph &graph) {

}

void VisgraphGenerator::sort_verticies_by_position(std::vector<Coordinate> &vertices) {
    const auto cmp = [](const Coordinate &a, const Coordinate &b) {
        if (a.get_longitude() > b.get_longitude()) {
            return true;
        }
        return a.get_longitude() == b.get_longitude() && a.get_latitude() > b.get_latitude();
    };
    std::sort(vertices.begin(), vertices.end(), cmp);
}

std::vector<std::shared_ptr<RotationTreeNode>>
VisgraphGenerator::create_vertex_nodes(const std::vector<Coordinate> &vertices) {
    auto vertex_nodes = std::vector<std::shared_ptr<RotationTreeNode>>();
    vertex_nodes.reserve(vertices.size());

    for (const auto &vertex : vertices) {
        auto vertex_node = std::make_shared<RotationTreeNode>(vertex);
        vertex_nodes.push_back(vertex_node);
    }

    return vertex_nodes;
}
