import os
import typing

from haversine import haversine  # type: ignore

from capi.src.implementation.datastructures.graph_file_paths import GraphFilePaths
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
        intersection_prechecker_factory: typing.Optional[IIntersectionPrecheckerFactory] = None,
    ):
        graph_paths = GraphFilePaths(visibility_graph_file_path)
        self._graph = load_graph_from_file(graph_paths.default_graph_path)

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

    def _get_shortest_path(self, start: VisGraphCoord, end: VisGraphCoord) -> typing.Sequence[Coordinate]:
        path = self._graph.shortest_path(start, end)
        n = self._graph.get_neighbors(path[1])
        return self._convert_visgraph_coords_list_to_coordinates(path)

    @staticmethod
    def _convert_visgraph_coords_list_to_coordinates(
        points: typing.Sequence[VisGraphCoord],
    ) -> typing.Sequence[Coordinate]:
        return [Coordinate.construct(latitude=point.latitude, longitude=point.longitude) for point in points]
