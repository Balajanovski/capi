import typing

from haversine import haversine  # type: ignore

from capi.interfaces.path_interpolator import IPathInterpolator
from capi.src.dtos.coordinate import Coordinate
from capi.src.pyvisgraph.graph import Point
from capi.src.pyvisgraph.vis_graph import VisGraph


class PathInterpolator(IPathInterpolator):
    def __init__(
        self,
        shapefile_file_path: str,
        visibility_graph_file_path: str,
        meridian_crossing_visibility_graph_file_path: typing.Optional[str] = None,
    ):
        self._graph = VisGraph()
        self._graph.load(visibility_graph_file_path)

        self._meridian_crossing_graph: typing.Optional[VisGraph] = None
        if meridian_crossing_visibility_graph_file_path is not None:
            self._meridian_crossing_graph = VisGraph()
            self._meridian_crossing_graph.load(meridian_crossing_visibility_graph_file_path)

    def interpolate(self, coord_1: Coordinate, coord_2: Coordinate) -> typing.Sequence[Coordinate]:
        point_1 = Point(coord_1.longitude, coord_1.latitude)
        point_2 = Point(coord_2.longitude, coord_2.latitude)

        return self._convert_to_points_list_to_coordinates(self._get_shortest_path(point_1, point_2))

    def _get_shortest_path(self, start: Point, end: Point) -> typing.Sequence[Point]:
        path = self._graph.shortest_path(start, end)
        if self._meridian_crossing_graph is None:
            return path

        meridian_path = self._meridian_crossing_graph.shortest_path(
            Point(((start.x + 270) % 360) - 180, start.y),
            Point(((end.x + 270) % 360) - 180, end.y),
        )

        if self._get_path_length(meridian_path) < self._get_path_length(path):
            return [Point(((point.x + 90) % 360) - 180, point.y) for point in meridian_path]
        return path

    @staticmethod
    def _get_path_length(path: typing.Sequence[Point]) -> float:
        length = 0
        for i in range(1, len(path)):
            length += haversine((path[i - 1].y, path[i - 1].x), (path[i].y, path[i].x))
        return length

    @staticmethod
    def _convert_to_points_list_to_coordinates(
        points: typing.Sequence[Point],
    ) -> typing.Sequence[Coordinate]:
        return [Coordinate.construct(latitude=point.y, longitude=point.x) for point in points]
