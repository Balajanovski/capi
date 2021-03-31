//
// Created by James.Balajan on 31/03/2021.
//

#include <stdexcept>

#include "rotation_tree_node.hpp"

RotationTreeNode::RotationTreeNode(const Coordinate &coord)
    : _coord(coord), _father(nullptr), _left_brother(nullptr), _right_brother(nullptr), _rightmost_son(nullptr) {}

void RotationTreeNode::add_node_as_rightmost_son(const std::shared_ptr<RotationTreeNode> &node) {
    if (node == nullptr) {
        throw std::runtime_error("Attempted to add null node as rightmost son");
    }

    node->detach_from_tree();

    if (_rightmost_son != nullptr)
        _rightmost_son->_right_brother = node;
    node->_left_brother = _rightmost_son;
    node->_father = shared_from_this();

    _rightmost_son = node;
}

void RotationTreeNode::add_node_as_left_brother(const std::shared_ptr<RotationTreeNode> &node) {
    if (node == nullptr) {
        throw std::runtime_error("Attempted to add null node as left brother");
    }
    if (node->_father == nullptr) {
        throw std::runtime_error("Attempted to add left brother to root node");
    }

    node->detach_from_tree();

    node->_father = _father;

    if (_left_brother != nullptr)
        _left_brother->_right_brother = node;
    node->_left_brother = _left_brother;

    node->_right_brother = shared_from_this();
    _left_brother = node;
}

void RotationTreeNode::detach_from_tree() {
    if (_father != nullptr && _father->_rightmost_son == shared_from_this()) {
        _father->_rightmost_son = _left_brother;
    }

    if (_left_brother != nullptr)
        _left_brother->_right_brother = _right_brother;
    if (_right_brother != nullptr)
        _right_brother->_left_brother = _left_brother;

    _father = nullptr;
    _left_brother = nullptr;
    _right_brother = nullptr;
}

std::shared_ptr<RotationTreeNode> RotationTreeNode::get_right_brother() const { return _right_brother; }

std::shared_ptr<RotationTreeNode> RotationTreeNode::get_left_brother() const { return _left_brother; }

std::shared_ptr<RotationTreeNode> RotationTreeNode::get_father() const { return _father; }

std::shared_ptr<RotationTreeNode> RotationTreeNode::get_rightmost_son() const { return _rightmost_son; }

Coordinate RotationTreeNode::get_coordinate() const { return _coord; }
