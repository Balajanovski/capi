import os
import typing
import unittest

from haversine import haversine  # type: ignore

from capi.src.implementation.dtos.coordinate import Coordinate
from capi.src.implementation.path_interpolator import PathInterpolator
from capi.test.test_files.test_files_dir import TEST_FILES_DIR


class TestPathInterpolator(unittest.TestCase):
    _GRAPH_FILE_PATH = os.path.join(TEST_FILES_DIR, "graph")

    def test_copenhagen_to_singapore(self):
        copenhagen_coordinates = Coordinate(latitude=55, longitude=13)
        singapore_coordinates = Coordinate(latitude=1, longitude=104)

        expected_path = [
            self._make_coordinate(13.00, 55.00),
            self._make_coordinate(12.68, 55.59),
            self._make_coordinate(12.31, 56.13),
            self._make_coordinate(10.66, 57.74),
            self._make_coordinate(8.60, 57.12),
            self._make_coordinate(1.39, 51.15),
            self._make_coordinate(-1.94, 49.73),
            self._make_coordinate(-4.78, 48.51),
            self._make_coordinate(-9.30, 43.05),
            self._make_coordinate(-9.50, 38.78),
            self._make_coordinate(-9.00, 37.02),
            self._make_coordinate(-5.61, 36.00),
            self._make_coordinate(3.90, 36.93),
            self._make_coordinate(9.74, 37.35),
            self._make_coordinate(11.04, 37.09),
            self._make_coordinate(31.92, 31.53),
            self._make_coordinate(32.28, 31.23),
            self._make_coordinate(32.57, 29.93),
            self._make_coordinate(43.46, 12.67),
            self._make_coordinate(50.80, 11.99),
            self._make_coordinate(80.26, 6.00),
            self._make_coordinate(95.23, 5.58),
            self._make_coordinate(97.50, 5.26),
            self._make_coordinate(103.51, 1.27),
            self._make_coordinate(103.97, 1.00),
            self._make_coordinate(104.00, 1.00),
        ]

        interpolator = PathInterpolator(
            visibility_graph_file_path=self._GRAPH_FILE_PATH,
        )
        path = interpolator.interpolate(copenhagen_coordinates, singapore_coordinates)

        self._assert_paths_equal(expected_path, path)

    def test_copenhagen_to_stockholm(self):
        copenhagen_coordinates = Coordinate(latitude=55, longitude=13)
        stockholm_coordinates = Coordinate(latitude=59.30, longitude=19.162)

        expected_path = [
            self._make_coordinate(13, 55),
            self._make_coordinate(14.1966, 55.382),
            self._make_coordinate(15.848332999, 56.073722),
            self._make_coordinate(18.71244399998, 59.278332999996),
            self._make_coordinate(19.162, 59.3),
        ]

        interpolator = PathInterpolator(
            visibility_graph_file_path=self._GRAPH_FILE_PATH,
        )
        path = interpolator.interpolate(copenhagen_coordinates, stockholm_coordinates)

        self._assert_paths_equal(expected_path, path)

    def test_50_km_distance(self):
        coords_1 = Coordinate(latitude=54.73928, longitude=10.73809)
        coords_2 = Coordinate(latitude=54.76034, longitude=10.66828)

        interpolator = PathInterpolator(
            visibility_graph_file_path=self._GRAPH_FILE_PATH,
        )
        path = interpolator.interpolate(coords_1, coords_2)

    def test_cross_meridian(self):
        coords_1 = Coordinate(latitude=1, longitude=104)
        coords_2 = Coordinate(latitude=37, longitude=-125)

        expected_path = [
            self._make_coordinate(104., 1.00),
            self._make_coordinate(116.74586, 7.034528),
            self._make_coordinate(123.012472, 9.040389),
            self._make_coordinate(123.451306, 9.194139),
            self._make_coordinate(126.06253, 10.0621),
            self._make_coordinate(-125.0, 37.0),
        ]

        interpolator = PathInterpolator(
            visibility_graph_file_path=self._GRAPH_FILE_PATH,
        )
        path = interpolator.interpolate(coords_1, coords_2)

        self._assert_paths_equal(expected_path, path)

    def test_non_intersecting(self):
        coords_1 = self._make_coordinate(-79.80740, 9.13017)
        coords_2 = self._make_coordinate(-79.81564, 9.14596)

        expected_path = [coords_1, coords_2]

        interpolator = PathInterpolator(
            visibility_graph_file_path=self._GRAPH_FILE_PATH,
        )
        path = interpolator.interpolate(coords_1, coords_2)

        self._assert_paths_equal(expected_path, path)

    @staticmethod
    def _make_coordinate(longitude: float, latitude: float) -> Coordinate:
        return Coordinate(
            latitude=latitude,
            longitude=longitude,
        )

    @staticmethod
    def _assert_paths_equal(
        path_1: typing.Sequence[Coordinate],
        path_2: typing.Sequence[Coordinate],
    ) -> None:
        dist = TestPathInterpolator._path_difference(path_1, path_2)
        tolerance = 160

        if dist > tolerance:
            raise AssertionError(f"Paths do not equal within tolerance of {tolerance}")

    @staticmethod
    def _path_difference(path1: typing.Sequence[Coordinate], path2: typing.Sequence[Coordinate]) -> float:
        if len(path1) < len(path2):
            s_path = path1
            l_path = path2
        else:
            s_path = path2
            l_path = path1

        path_diff_sum = 0
        for l_path_pt in l_path:
            shortest_dist_to_l_path = float("inf")
            for s_path_pt in s_path:
                shortest_dist_to_l_path = min(shortest_dist_to_l_path,
                                              haversine((l_path_pt.latitude, l_path_pt.longitude),
                                                        (s_path_pt.latitude, s_path_pt.longitude)))
            path_diff_sum += shortest_dist_to_l_path
        return path_diff_sum / len(s_path)
