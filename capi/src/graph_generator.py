import shapefile  # type: ignore

from capi.interfaces.graph_generator import IGraphGenerator
from capi.src.pyvisgraph.graph import Point
from capi.src.pyvisgraph.vis_graph import VisGraph


class GraphGenerator(IGraphGenerator):
    def __init__(self, num_workers: int = 1):
        self._num_workers = num_workers

    def generate(self, shape_file_path: str, output_path: str, meridian_crossing: bool = False) -> None:
        shape_file = shapefile.Reader(shape_file_path)
        polygons = [
            [
                self._generate_point_with_meridian_adjustment(point[0], point[1], meridian_crossing)
                for point in shape.points
            ]
            for shape in shape_file.shapes()
        ]

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


if __name__ == "__main__":
    import os

    from capi.test.test_files.test_files_dir import TEST_FILES_DIR

    gen = GraphGenerator(num_workers=32)
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
        os.path.join(TEST_FILES_DIR, "GSHHS_c_L1.shp"),
        os.path.join(TEST_FILES_DIR, "graph.pkl"),
        meridian_crossing=False,
    )
    gen.generate(
        os.path.join(TEST_FILES_DIR, "GSHHS_c_L1.shp"),
        os.path.join(TEST_FILES_DIR, "meridian_graph.pkl"),
        meridian_crossing=True,
    )
