import os
import time
import typing
from functools import reduce

from capi.src.implementation.datastructures.graph_file_paths import GraphFilePaths
from capi.src.implementation.shapefiles.shapefile_reader import ShapefileReader
from capi.src.implementation.visibility_graphs import (
    VisGraphCoord,
    VisGraphPolygon,
    generate_visgraph,
    generate_visgraph_with_shuffled_range,
    save_graph_to_file,
)
from capi.src.interfaces.graph_generator import IGraphGenerator
from capi.src.interfaces.shapefiles.shapefile_reader import IShapefileReader


class GraphGenerator(IGraphGenerator):
    def __init__(self, shapefile_reader: typing.Optional[IShapefileReader] = None):
        self._shapefile_reader = ShapefileReader() if shapefile_reader is None else shapefile_reader

    def generate(self, shape_file_path: str, output_path: str) -> None:
        os.mkdir(output_path)
        graph_file = GraphFilePaths(output_path)

        curr_file_output_path = graph_file.default_graph_path

        polygons = self._read_polygons_from_shapefile(shape_file_path)

        graph = generate_visgraph(polygons)

        save_graph_to_file(graph, curr_file_output_path)

    def generate_for_vertex_range(
        self, shape_file_path: str, output_path: str, current_split_num: int, num_splits: int, seed: int
    ) -> None:
        os.mkdir(output_path)
        graph_file = GraphFilePaths(output_path)

        curr_file_output_path = graph_file.default_graph_path

        polygons = self._read_polygons_from_shapefile(shape_file_path)
        num_vertices = self._get_num_vertices_in_polygons(polygons)
        split_size = num_vertices // num_splits

        split_start = split_size * current_split_num
        split_end = (split_size * (current_split_num + 1)) if current_split_num < num_splits - 1 else num_vertices

        graph = generate_visgraph_with_shuffled_range(polygons, split_start, split_end, seed)

        save_graph_to_file(graph, curr_file_output_path)

    def _read_polygons_from_shapefile(self, shape_file_path: str) -> typing.Sequence[VisGraphPolygon]:
        read_polygons = self._shapefile_reader.read(shape_file_path)
        unadjusted_polygons = [
            VisGraphPolygon([VisGraphCoord(vertex.longitude, vertex.latitude) for vertex in polygon.vertices])
            for polygon in read_polygons
        ]

        return unadjusted_polygons

    @staticmethod
    def _get_num_vertices_in_polygons(polygons: typing.Sequence[VisGraphPolygon]) -> int:
        return reduce(lambda a, b: a + b, map(lambda polygon: len(polygon.vertices), polygons))


if __name__ == "__main__":
    from capi.test.test_files.test_files_dir import TEST_FILES_DIR

    gen = GraphGenerator()

    start_time = time.time()

    gen.generate(
        os.path.join(TEST_FILES_DIR, "smaller.shp"),
        os.path.join(TEST_FILES_DIR, "smaller_graph"),
    )

    end_time = time.time()
    print(f"Time taken for smaller: {end_time - start_time}")

    start_time = time.time()

    gen.generate(
        os.path.join(TEST_FILES_DIR, "GSHHS_c_L1.shp"),
        os.path.join(TEST_FILES_DIR, "graph"),
    )

    end_time = time.time()
    print(f"Time taken for larger: {end_time - start_time}")

    gen.generate_for_vertex_range(
        os.path.join(TEST_FILES_DIR, "smaller.shp"),
        os.path.join(TEST_FILES_DIR, "smaller_graph_range_1"),
        0,
        2,
        42,
    )

    gen.generate_for_vertex_range(
        os.path.join(TEST_FILES_DIR, "smaller.shp"),
        os.path.join(TEST_FILES_DIR, "smaller_graph_range_2"),
        1,
        2,
        42,
    )
