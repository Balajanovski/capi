//
// Created by James.Balajan on 8/06/2021.
//

#include <iostream>
#include <cmath>
#include <ctime>

#include "types/coordinate/coordinate.hpp"
#include "serialization/graph_serializer.hpp"
#include "shortest_path/shortest_path_computer.hpp"

int main() {
    const auto graph = GraphSerializer::deserialize_from_file("/mnt/d/DATVM/vessel_eta_prediction/resources/shapefiles/graph/default");
    const auto computer = ShortestPathComputer(graph);

    const auto start = std::time(nullptr);
    const auto paths = computer.shortest_paths(
            std::vector<std::pair<Coordinate, Coordinate>> {
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.763333, 23.005)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.763333, 23.005)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.763333, 23.005)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.763333, 23.005)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.763333, 23.005)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.763333, 23.005)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.763333, 23.005)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.763333, 23.005)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.763333, 23.005)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.693333, 23.08)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.556155, 23.135242)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.366667, 23.155)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.343333, 23.131667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.343333, 23.133333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.343333, 23.133333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.343333, 23.131667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.343333, 23.133333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.343333, 23.133333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.343333, 23.133333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.343333, 23.133333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.341667, 23.133333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.343333, 23.133333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.341667, 23.133333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.343333, 23.133333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.341667, 23.131667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.341667, 23.131667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.341667, 23.131667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.341667, 23.131667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.341667, 23.131667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.341667, 23.131667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.341667, 23.131667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.341667, 23.131667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.341667, 23.131667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.343528, 23.137045)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.43, 23.168333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.525, 23.16)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.631667, 23.148333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.736667, 23.135)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.865, 23.125)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.996667, 23.115)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.12, 23.105)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.253333, 23.096667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.368333, 23.088333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.465, 23.041667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.555, 22.981667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.645, 22.928333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.738333, 22.896667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.816667, 22.878333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.883333, 22.87)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.986667, 22.865)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.985, 22.778333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.7)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.7)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.7)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.7)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.975, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.7)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.701667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.7)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.975, 22.7)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.967772, 22.673523)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.966667, 22.673333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.981667, 22.741667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.765)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.765)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.765)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.765)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.765)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.765)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.765)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.973333, 22.765)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.896667, 22.858333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.743333, 22.916667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.59, 22.955)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.456667, 22.986667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.316667, 23.023333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.183333, 23.041667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-83.031667, 23.045)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.891667, 23.051667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.776667, 23.05)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.76, 23.006667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753333, 22.995)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.753802, 22.999397)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.763333, 23.011667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.763333, 23.011667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.763333, 23.011667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.763333, 23.011667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.763333, 23.011667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.733333, 23.056667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.325, 23.231667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.2, 23.226667)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-82.065793, 23.221057)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-81.923333, 23.213333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-81.761667, 23.205)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-81.613333, 23.198333)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-81.51, 23.095)),
                    std::make_pair(Coordinate(-81.5376333, 23.0577), Coordinate(-81.551667, 23.056667)),
                },
            INFINITY,
            false,
            1.0);
    std::cout << "Took " << std::time(nullptr) - start << " seconds" << std::endl;

    /*
    for (const auto &path : paths) {
        if (!path.has_value()) {
            continue;
        }

        std::cout << "Path:\n";
        for (const auto &coord : path.value()) {
            std::cout << coord << "\n";
        }
    }
    std::cout << std::flush;*/
}