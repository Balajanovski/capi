//
// Created by James.Balajan on 31/03/2021.
//

#ifndef CAPI_ROTATION_TREE_NODE_HPP
#define CAPI_ROTATION_TREE_NODE_HPP

#include <memory>

#include "types/coordinate/coordinate.hpp"

class RotationTreeNode : std::enable_shared_from_this<RotationTreeNode> {
  public:
    RotationTreeNode(const Coordinate &coord);

    void add_node_as_rightmost_son(const std::shared_ptr<RotationTreeNode> &node);
    void add_node_as_left_brother(const std::shared_ptr<RotationTreeNode> &node);
    void detach_from_tree();

    std::shared_ptr<RotationTreeNode> get_right_brother() const;
    std::shared_ptr<RotationTreeNode> get_left_brother() const;
    std::shared_ptr<RotationTreeNode> get_father() const;
    std::shared_ptr<RotationTreeNode> get_rightmost_son() const;
    Coordinate get_coordinate() const;

  private:
    Coordinate _coord;

    std::shared_ptr<RotationTreeNode> _father;
    std::shared_ptr<RotationTreeNode> _left_brother;
    std::shared_ptr<RotationTreeNode> _right_brother;
    std::shared_ptr<RotationTreeNode> _rightmost_son;
};

#endif // CAPI_ROTATION_TREE_NODE_HPP
