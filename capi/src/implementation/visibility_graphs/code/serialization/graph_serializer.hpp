//
// Created by James.Balajan on 11/05/2021.
//

#ifndef CAPI_GRAPH_SERIALIZER_HPP
#define CAPI_GRAPH_SERIALIZER_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <mio.hpp>

#include "datastructures/graph/graph.hpp"


class GraphSerializer {
  public:
    static void serialize_to_file(const Graph& graph, const std::string& path);
    static Graph deserialize_from_file(const std::string& path);
  private:
    static size_t calculate_number_of_bytes_for_graph(const Graph& graph);
    static size_t serialize_polygon_vertices_to_mmap(mio::mmap_sink& mmap, const Graph& graph, size_t offset);
    static size_t serialize_adjacency_matrix_to_mmap(mio::mmap_sink& mmap, const Graph& graph, size_t offset);
};

#endif // CAPI_GRAPH_SERIALIZER_HPP