import typing

from capi.src.implementation.datastructures.graph_file_paths import GraphFilePaths
from capi.src.implementation.dtos.coordinate import Coordinate
from capi.src.implementation.visibility_graphs import VisGraphCoord, VisGraphShortestPathComputer, load_graph_from_file
from capi.src.interfaces.path_interpolator import IPathInterpolator


class PathInterpolator(IPathInterpolator):
    def __init__(
        self,
        visibility_graph_file_path: str,
    ):
        graph_paths = GraphFilePaths(visibility_graph_file_path)
        graph = load_graph_from_file(graph_paths.default_graph_path)

        self._shortest_path_computer = VisGraphShortestPathComputer(graph)

    def interpolate(self, coord_1: Coordinate, coord_2: Coordinate) -> typing.Sequence[Coordinate]:
        point_1 = VisGraphCoord(coord_1.longitude, coord_1.latitude)
        point_2 = VisGraphCoord(coord_2.longitude, coord_2.latitude)

        path = self._get_shortest_path(point_1, point_2)

        return path

    def _get_shortest_path(self, start: VisGraphCoord, end: VisGraphCoord) -> typing.Sequence[Coordinate]:
        path = self._shortest_path_computer.shortest_path(start, end)
        return self._convert_visgraph_coords_list_to_coordinates(path)

    @staticmethod
    def _convert_visgraph_coords_list_to_coordinates(
        points: typing.Sequence[VisGraphCoord],
    ) -> typing.Sequence[Coordinate]:
        return [Coordinate.construct(latitude=point.latitude, longitude=point.longitude) for point in points]
