import os
import typing
from functools import reduce

from capi.src.implementation.shapefiles.shapefile_reader import ShapefileReader
from capi.src.implementation.visibility_graphs import (
    VisGraph,
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

        for meridian_crossing in [False, True]:
            curr_file_output_path = os.path.join(output_path, "meridian" if meridian_crossing else "default")

            unadjusted_polygons = self._read_polygons_from_shapefile(shape_file_path, meridian_crossing)
            polygons = self._split_meridian_crossing_polygons(unadjusted_polygons)

            graph = generate_visgraph(polygons)

            save_graph_to_file(graph, curr_file_output_path)

    def generate_for_vertex_range(
        self, shape_file_path: str, output_path: str, current_split_num: int, num_splits: int, seed: int
    ) -> None:
        os.mkdir(output_path)

        for meridian_crossing in [False, True]:
            curr_file_output_path = os.path.join(output_path, "meridian" if meridian_crossing else "default")

            unadjusted_polygons = self._read_polygons_from_shapefile(shape_file_path, meridian_crossing)
            polygons = self._split_meridian_crossing_polygons(unadjusted_polygons)
            num_vertices = self._get_num_vertices_in_polygons(polygons)
            split_size = num_vertices // num_splits

            split_start = split_size * current_split_num
            split_end = (split_size * (current_split_num + 1)) if current_split_num < num_splits - 1 else num_vertices

            graph = generate_visgraph_with_shuffled_range(polygons, split_start, split_end, seed)

            save_graph_to_file(graph, curr_file_output_path)

    def _split_meridian_crossing_polygons(
        self, unadjusted_polygons: typing.Sequence[VisGraphPolygon]
    ) -> typing.Sequence[VisGraphPolygon]:
        polygons: typing.List[VisGraphPolygon] = []
        for unadjusted_polygon in unadjusted_polygons:
            if self._is_polygon_crossing_meridian(unadjusted_polygon):
                split_poly_1, split_poly_2 = self._split_polygon_crossing_meridian(unadjusted_polygon)
                polygons.append(split_poly_1)
                polygons.append(split_poly_2)
            else:
                polygons.append(unadjusted_polygon)

        return polygons

    def _read_polygons_from_shapefile(
        self, shape_file_path: str, meridian_crossing: bool
    ) -> typing.Sequence[VisGraphPolygon]:
        read_polygons = self._shapefile_reader.read(shape_file_path)
        unadjusted_polygons = [
            VisGraphPolygon(
                [
                    self._generate_point_with_meridian_adjustment(vertex.longitude, vertex.latitude, meridian_crossing)
                    for vertex in polygon.vertices
                ]
            )
            for polygon in read_polygons
        ]

        return unadjusted_polygons

    @staticmethod
    def _get_num_vertices_in_polygons(polygons: typing.Sequence[VisGraphPolygon]) -> int:
        return reduce(lambda a, b: a + b, map(lambda polygon: len(polygon.vertices), polygons))

    @staticmethod
    def _generate_point_with_meridian_adjustment(
        longitude: float, latitude: float, meridian_crossing: bool
    ) -> VisGraphCoord:
        if meridian_crossing:
            return VisGraphCoord(
                ((longitude + 270) % 360) - 180,
                latitude,
            )
        return VisGraphCoord(longitude, latitude)

    @staticmethod
    def _is_polygon_crossing_meridian(polygon: VisGraphPolygon) -> bool:
        if len(polygon.vertices) <= 2:
            return False

        for i in range(len(polygon.vertices) - 1):
            longitudinal_distance = abs(polygon.vertices[i].longitude - polygon.vertices[i + 1].longitude)

            if longitudinal_distance > 180:
                return True

        return False

    @staticmethod
    def _split_polygon_crossing_meridian(
        polygon: VisGraphPolygon,
    ) -> typing.Tuple[VisGraphPolygon, VisGraphPolygon]:
        polygons: typing.List[typing.List[VisGraphCoord]] = [[polygon.vertices[0]], []]

        curr_polygon_bucket_index = 0
        for i in range(len(polygon.vertices) - 1):
            if abs(polygon.vertices[i].longitude - polygon.vertices[i + 1].longitude) > 180:
                curr_polygon_bucket_index = (curr_polygon_bucket_index + 1) % 2
            polygons[curr_polygon_bucket_index].append(polygon.vertices[i + 1])

        return VisGraphPolygon(polygons[0]), VisGraphPolygon(polygons[1])


if __name__ == "__main__":
    import os
    import time

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
        10,
        42,
    )

    gen.generate_for_vertex_range(
        os.path.join(TEST_FILES_DIR, "smaller.shp"),
        os.path.join(TEST_FILES_DIR, "smaller_graph_range_2"),
        9,
        10,
        42,
    )
