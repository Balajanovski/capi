//
// Created by James.Balajan on 11/05/2021.
//

#ifndef CAPI_GRAPH_SERIALIZER_HPP
#define CAPI_GRAPH_SERIALIZER_HPP

#include <cstdint>
#include <mio.hpp>
#include <string>
#include <vector>
#include <memory>


#include "datastructures/graph/graph.hpp"

class GraphSerializer {
  public:
    static void serialize_to_file(const std::shared_ptr<Graph> &graph, const std::string &path);
    static std::shared_ptr<Graph> deserialize_from_file(const std::string &path);

  private:
    static size_t calculate_number_of_bytes_for_graph(const std::shared_ptr<Graph> &graph, uint64_t num_meridian_spanning_edges);
    static uint64_t calculate_number_of_meridian_spanning_edges_for_graph(const std::shared_ptr<Graph> &graph);

    static size_t serialize_polygon_vertices_to_mmap(mio::mmap_sink &mmap, const std::shared_ptr<Graph> &graph, size_t offset);
    static size_t serialize_adjacency_matrix_to_mmap(mio::mmap_sink &mmap, const std::shared_ptr<Graph> &graph, size_t offset,
                                                     uint64_t num_meridian_spanning_edges);

    static size_t deserialize_polygon_vertices_from_mmap(const mio::mmap_source &mmap, std::shared_ptr<Graph> &graph,
                                                         uint64_t num_polygons, size_t offset);
    static size_t deserialize_adjacency_matrix_from_mmap(const mio::mmap_source &mmap, std::shared_ptr<Graph> &graph, size_t offset);
};

#endif // CAPI_GRAPH_SERIALIZER_HPP
