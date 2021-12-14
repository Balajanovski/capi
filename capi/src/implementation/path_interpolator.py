import math
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

    def interpolate(
        self,
        coord_1: Coordinate,
        coord_2: Coordinate,
        search_distance_from_source_limit: float = math.inf,
        correct_vertices_on_land: bool = False,
        a_star_greediness_weighting: float = 1.0,
    ) -> typing.Sequence[Coordinate]:
        point_1 = VisGraphCoord(coord_1.longitude, coord_1.latitude)
        point_2 = VisGraphCoord(coord_2.longitude, coord_2.latitude)

        path = self._get_shortest_path(
            point_1, point_2, search_distance_from_source_limit, correct_vertices_on_land, a_star_greediness_weighting
        )

        return path

    def batch_interpolate(
        self,
        coord_pairs: typing.Sequence[typing.Tuple[Coordinate, Coordinate]],
        search_distance_from_source_limit: float = math.inf,
        correct_vertices_on_land: bool = False,
        a_star_greediness_weighting: float = 1.0,
    ) -> typing.Sequence[typing.Optional[typing.Sequence[Coordinate]]]:
        _coord_pairs = [
            (
                VisGraphCoord(pair[0].longitude, pair[0].latitude),
                VisGraphCoord(pair[1].longitude, pair[1].latitude),
            )
            for pair in coord_pairs
        ]

        return self._batch_get_shortest_path(
            _coord_pairs, search_distance_from_source_limit, correct_vertices_on_land, a_star_greediness_weighting
        )

    def _get_shortest_path(
        self,
        start: VisGraphCoord,
        end: VisGraphCoord,
        search_distance_from_source_limit: float,
        correct_vertices_on_land: bool,
        a_star_greediness_weighting: float,
    ) -> typing.Sequence[Coordinate]:
        path = self._shortest_path_computer.shortest_path(
            start,
            end,
            search_distance_from_source_limit,
            correct_vertices_on_land,
            a_star_greediness_weighting,
        )
        return self._convert_visgraph_coords_list_to_coordinates(path)

    def _batch_get_shortest_path(
        self,
        coord_pairs: typing.Sequence[typing.Tuple[VisGraphCoord, VisGraphCoord]],
        search_distance_from_source_limit: float,
        correct_vertices_on_land: bool,
        a_star_greediness_weighting: float,
    ) -> typing.Sequence[typing.Optional[typing.Sequence[Coordinate]]]:
        paths = self._shortest_path_computer.shortest_paths(
            coord_pairs, search_distance_from_source_limit, correct_vertices_on_land, a_star_greediness_weighting
        )

        return [self._convert_visgraph_coords_list_to_coordinates(path) if path is not None else None for path in paths]

    @staticmethod
    def _convert_visgraph_coords_list_to_coordinates(
        points: typing.Sequence[VisGraphCoord],
    ) -> typing.Sequence[Coordinate]:
        return [Coordinate(latitude=point.latitude, longitude=point.longitude) for point in points]
