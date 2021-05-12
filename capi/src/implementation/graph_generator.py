import os
import typing

from capi.src.implementation.shapefiles.shapefile_reader import ShapefileReader
from capi.src.implementation.visibility_graphs import (
    VisGraphCoord,
    VisGraphPolygon,
    generate_visgraph,
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

            read_polygons = self._shapefile_reader.read(shape_file_path)
            unadjusted_polygons = [
                VisGraphPolygon(
                    [
                        self._generate_point_with_meridian_adjustment(
                            vertex.longitude, vertex.latitude, meridian_crossing
                        )
                        for vertex in polygon.vertices
                    ]
                )
                for polygon in read_polygons
            ]

            polygons: typing.List[VisGraphPolygon] = []
            for unadjusted_polygon in unadjusted_polygons:
                if self._is_polygon_crossing_meridian(unadjusted_polygon):
                    split_poly_1, split_poly_2 = self._split_polygon_crossing_meridian(unadjusted_polygon)
                    polygons.append(split_poly_1)
                    polygons.append(split_poly_2)
                else:
                    polygons.append(unadjusted_polygon)

            graph = generate_visgraph(polygons)

            save_graph_to_file(graph, curr_file_output_path)

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
