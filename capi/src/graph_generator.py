import typing

import shapefile  # type: ignore

from capi.interfaces.graph_generator import IGraphGenerator
from capi.src.pyvisgraph.graph import Point
from capi.src.pyvisgraph.vis_graph import VisGraph


class GraphGenerator(IGraphGenerator):
    def __init__(self, num_workers: int = 1):
        self._num_workers = num_workers

    def generate(self, shape_file_path: str, output_path: str, meridian_crossing: bool = False) -> None:
        shape_file = shapefile.Reader(shape_file_path)
        unadjusted_polygons = [
            [
                self._generate_point_with_meridian_adjustment(point[0], point[1], meridian_crossing)
                for point in shape.points
            ]
            for shape in shape_file.shapes()
        ]

        polygons: typing.List[typing.Sequence[Point]] = []
        for unadjusted_polygon in unadjusted_polygons:
            if self._is_polygon_crossing_meridian(unadjusted_polygon):
                split_poly_1, split_poly_2 = self._split_polygon_crossing_meridian(unadjusted_polygon)
                polygons.append(split_poly_1)
                polygons.append(split_poly_2)
            else:
                polygons.append(unadjusted_polygon)

        graph = VisGraph()
        graph.build(polygons, workers=self._num_workers)
        graph.save(output_path)

    @staticmethod
    def _generate_point_with_meridian_adjustment(longitude: float, latitude: float, meridian_crossing: bool) -> Point:
        if meridian_crossing:
            return Point(
                ((longitude + 270) % 360) - 180,
                latitude,
            )
        return Point(longitude, latitude)

    @staticmethod
    def _is_polygon_crossing_meridian(polygon: typing.Sequence[Point]) -> bool:
        if len(polygon) <= 2:
            return False

        for i in range(len(polygon) - 1):
            longitudinal_distance = abs(polygon[i].x - polygon[i + 1].x)

            if longitudinal_distance > 180:
                return True

        return False

    @staticmethod
    def _split_polygon_crossing_meridian(
        polygon: typing.Sequence[Point],
    ) -> typing.Tuple[typing.Sequence[Point], typing.Sequence[Point]]:
        polygons: typing.List[typing.List[Point]] = [[polygon[0]], []]

        curr_polygon_bucket_index = 0
        for i in range(len(polygon) - 1):
            if abs(polygon[i].x - polygon[i + 1].x) >= 180:
                curr_polygon_bucket_index = (curr_polygon_bucket_index + 1) % 2
            polygons[curr_polygon_bucket_index].append(polygon[i + 1])

        return polygons[0], polygons[1]


if __name__ == "__main__":
    import os

    from capi.test.test_files.test_files_dir import TEST_FILES_DIR

    gen = GraphGenerator(num_workers=24)

    gen.generate(
        os.path.join(TEST_FILES_DIR, "smaller.shp"),
        os.path.join(TEST_FILES_DIR, "smaller_graph.pkl"),
        meridian_crossing=False,
    )
    gen.generate(
        os.path.join(TEST_FILES_DIR, "smaller.shp"),
        os.path.join(TEST_FILES_DIR, "smaller_meridian_graph.pkl"),
        meridian_crossing=True,
    )
    gen.generate(
        os.path.join(TEST_FILES_DIR, "GSHHS_l_L1.shp"),
        os.path.join(TEST_FILES_DIR, "l_graph.pkl"),
        meridian_crossing=False,
    )
    gen.generate(
        os.path.join(TEST_FILES_DIR, "GSHHS_l_L1.shp"),
        os.path.join(TEST_FILES_DIR, "l_meridian_graph.pkl"),
        meridian_crossing=True,
    )
