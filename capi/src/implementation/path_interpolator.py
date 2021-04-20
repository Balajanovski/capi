import typing

from haversine import haversine  # type: ignore

from capi.src.implementation.dtos.coordinate import Coordinate
from capi.src.implementation.intersection_prechecker_factory import IntersectionPrecheckerFactory
from capi.src.implementation.visibility_graphs import VisGraph, VisGraphCoord, load_graph_from_file
from capi.src.interfaces.intersection_prechecker_factory import IIntersectionPrecheckerFactory
from capi.src.interfaces.path_interpolator import IPathInterpolator


class PathInterpolator(IPathInterpolator):
    def __init__(
        self,
        shapefile_file_path: str,
        visibility_graph_file_path: str,
        meridian_crossing_visibility_graph_file_path: typing.Optional[str] = None,
        intersection_prechecker_factory: typing.Optional[IIntersectionPrecheckerFactory] = None,
    ):
        self._graph = load_graph_from_file(visibility_graph_file_path)

        self._meridian_crossing_graph: typing.Optional[VisGraph] = None
        if meridian_crossing_visibility_graph_file_path is not None:
            self._meridian_crossing_graph = load_graph_from_file(meridian_crossing_visibility_graph_file_path)

        _intersection_prechecker_factory = (
            IntersectionPrecheckerFactory()
            if intersection_prechecker_factory is None
            else intersection_prechecker_factory
        )
        self._intersection_prechecker = _intersection_prechecker_factory.create(shapefile_file_path)

    def interpolate(self, coord_1: Coordinate, coord_2: Coordinate) -> typing.Sequence[Coordinate]:
        if not self._intersection_prechecker.does_line_cross_polygons(coord_1, coord_2):
            return [coord_1, coord_2]

        point_1 = VisGraphCoord(coord_1.longitude, coord_1.latitude)
        point_2 = VisGraphCoord(coord_2.longitude, coord_2.latitude)

        return self._get_shortest_path(point_1, point_2)

    def _get_shortest_path(self, start: Coordinate, end: Coordinate) -> typing.Sequence[Coordinate]:
        path = self._graph.shortest_path(start, end)
        if self._meridian_crossing_graph is None:
            return self._convert_visgraph_coords_list_to_coordinates(path)

        meridian_path = self._meridian_crossing_graph.shortest_path(
            VisGraphCoord(((start.longitude + 270) % 360) - 180, start.latitude),
            VisGraphCoord(((end.longitude + 270) % 360) - 180, end.latitude),
        )

        if self._get_path_length(meridian_path) < self._get_path_length(path):
            return [
                Coordinate(longitude=((point.longitude + 90) % 360) - 180, latitude=point.latitude)
                for point in meridian_path
            ]
        return self._convert_visgraph_coords_list_to_coordinates(path)

    @staticmethod
    def _get_path_length(path: typing.Sequence[VisGraphCoord]) -> float:
        length = 0
        for i in range(1, len(path)):
            length += haversine((path[i - 1].latitude, path[i - 1].longitude), (path[i].latitude, path[i].longitude))
        return length

    @staticmethod
    def _convert_visgraph_coords_list_to_coordinates(
        points: typing.Sequence[VisGraphCoord],
    ) -> typing.Sequence[Coordinate]:
        return [Coordinate.construct(latitude=point.latitude, longitude=point.longitude) for point in points]
