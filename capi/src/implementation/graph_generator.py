import typing

from capi.src.implementation.shapefiles.shapefile_reader import ShapefileReader
from capi.src.implementation.visibility_graphs import VisGraphCoord, VisGraphPolygon, generate_visgraph
from capi.src.interfaces.graph_generator import IGraphGenerator
from capi.src.interfaces.shapefiles.shapefile_reader import IShapefileReader


class GraphGenerator(IGraphGenerator):
    def __init__(self, shapefile_reader: typing.Optional[IShapefileReader] = None):
        self._shapefile_reader = ShapefileReader() if shapefile_reader is None else shapefile_reader

    def generate(self, shape_file_path: str, output_path: str, meridian_crossing: bool = False) -> None:
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

        polygons: typing.List[VisGraphPolygon] = []
        for unadjusted_polygon in unadjusted_polygons:
            if self._is_polygon_crossing_meridian(unadjusted_polygon):
                split_poly_1, split_poly_2 = self._split_polygon_crossing_meridian(unadjusted_polygon)
                polygons.append(split_poly_1)
                polygons.append(split_poly_2)
            else:
                polygons.append(unadjusted_polygon)

        graph = generate_visgraph(polygons)
        graph.serialize_to_file(output_path)

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

    from capi.src.implementation.visibility_graphs import load_graph_from_file
    from capi.test.test_files.test_files_dir import TEST_FILES_DIR

    gen = GraphGenerator()

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
        os.path.join(TEST_FILES_DIR, "meridian_graph.pkl"),
        meridian_crossing=True,
    )
    gen.generate(
        os.path.join(TEST_FILES_DIR, "GSHHS_c_L1.shp"),
        os.path.join(TEST_FILES_DIR, "graph.pkl"),
        meridian_crossing=False,
    )

    """
    import folium

    map = folium.Map(location=[0, 0], zoom_start=0)

    g = load_graph_from_file(os.path.join(TEST_FILES_DIR, "graph.pkl"))
    vertex = VisGraphCoord(0, 38.931832)
    for neighbor in g.get_neighbors(vertex):
        folium.PolyLine([(vertex.latitude, vertex.longitude), (neighbor.latitude, neighbor.longitude)], color="red", weight=2.5, opacity=1).add_to(map)

    for polygon in g.polygons:
        points = [(vertex.latitude, vertex.longitude) for vertex in polygon.vertices]
        folium.Polygon(points, color="green", weight=2.5, opacity=1).add_to(map)

    map.save(os.path.join(TEST_FILES_DIR, "index.html"))
    """
