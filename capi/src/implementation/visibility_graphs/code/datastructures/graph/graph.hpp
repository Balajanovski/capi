//
// Created by James.Balajan on 31/03/2021.
//

#ifndef CAPI_GRAPH_HPP
#define CAPI_GRAPH_HPP

#include <unordered_set>
#include <vector>

class Graph {
  public:
    explicit Graph(int num_vertices);

    void add_edge(int a, int b);
    bool has_edge(int a, int b) const;

  private:
    std::vector<std::unordered_set<int>> _neighbors;
    int _num_vertices;
};

#endif // CAPI_GRAPH_HPP
