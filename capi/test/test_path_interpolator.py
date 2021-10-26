import os
import typing
import unittest

from haversine import haversine  # type: ignore

from capi.src.implementation.dtos.coordinate import Coordinate
from capi.src.implementation.path_interpolator import PathInterpolator
from capi.test.test_files.test_files_dir import TEST_FILES_DIR


class TestPathInterpolator(unittest.TestCase):
    _GRAPH_FILE_PATH = os.path.join(TEST_FILES_DIR, "graph")
    _INTERPOLATOR = PathInterpolator(
        visibility_graph_file_path=_GRAPH_FILE_PATH,
    )

    _COPENHAGEN_COORDINATES = Coordinate(latitude=55, longitude=13)
    _SINGAPORE_COORDINATES = Coordinate(latitude=1, longitude=104)
    _STOCKHOLM_COORDINATES = Coordinate(latitude=59.30, longitude=19.162)
    _COPENHAGEN_TO_SINGAPORE_PATH: typing.List[Coordinate] = []
    _COPENHAGEN_TO_STOCKHOLM_PATH: typing.List[Coordinate] = []

    @classmethod
    def setUpClass(cls) -> None:
        cls._COPENHAGEN_TO_SINGAPORE_PATH = [
            TestPathInterpolator._make_coordinate(13.00, 55.00),
            TestPathInterpolator._make_coordinate(12.68, 55.59),
            TestPathInterpolator._make_coordinate(12.31, 56.13),
            TestPathInterpolator._make_coordinate(10.66, 57.74),
            TestPathInterpolator._make_coordinate(8.60, 57.12),
            TestPathInterpolator._make_coordinate(1.39, 51.15),
            TestPathInterpolator._make_coordinate(-1.94, 49.73),
            TestPathInterpolator._make_coordinate(-4.78, 48.51),
            TestPathInterpolator._make_coordinate(-9.30, 43.05),
            TestPathInterpolator._make_coordinate(-9.50, 38.78),
            TestPathInterpolator._make_coordinate(-9.00, 37.02),
            TestPathInterpolator._make_coordinate(-5.61, 36.00),
            TestPathInterpolator._make_coordinate(3.90, 36.93),
            TestPathInterpolator._make_coordinate(9.74, 37.35),
            TestPathInterpolator._make_coordinate(11.04, 37.09),
            TestPathInterpolator._make_coordinate(31.92, 31.53),
            TestPathInterpolator._make_coordinate(32.28, 31.23),
            TestPathInterpolator._make_coordinate(32.57, 29.93),
            TestPathInterpolator._make_coordinate(43.46, 12.67),
            TestPathInterpolator._make_coordinate(50.80, 11.99),
            TestPathInterpolator._make_coordinate(80.26, 6.00),
            TestPathInterpolator._make_coordinate(95.23, 5.58),
            TestPathInterpolator._make_coordinate(97.50, 5.26),
            TestPathInterpolator._make_coordinate(103.51, 1.27),
            TestPathInterpolator._make_coordinate(103.97, 1.00),
            TestPathInterpolator._make_coordinate(104.00, 1.00),
        ]

        cls._COPENHAGEN_TO_STOCKHOLM_PATH = [
            TestPathInterpolator._make_coordinate(13, 55),
            TestPathInterpolator._make_coordinate(14.1966, 55.382),
            TestPathInterpolator._make_coordinate(15.848332999, 56.073722),
            TestPathInterpolator._make_coordinate(18.71244399998, 59.278332999996),
            TestPathInterpolator._make_coordinate(19.162, 59.3),
        ]

    def test_copenhagen_to_singapore(self):
        path = self._INTERPOLATOR.interpolate(
            self._COPENHAGEN_COORDINATES, self._SINGAPORE_COORDINATES, a_star_greediness_weighting=1.1
        )

        self._assert_paths_equal(self._COPENHAGEN_TO_SINGAPORE_PATH, path)

    def test_copenhagen_to_singapore_distance_limit(self):
        with self.assertRaises(RuntimeError):
            self._INTERPOLATOR.interpolate(
                self._COPENHAGEN_COORDINATES,
                self._SINGAPORE_COORDINATES,
                search_distance_from_source_limit=1,
                a_star_greediness_weighting=1.1,
            )

    def test_copenhagen_to_stockholm(self):
        path = self._INTERPOLATOR.interpolate(
            self._COPENHAGEN_COORDINATES, self._STOCKHOLM_COORDINATES, a_star_greediness_weighting=1.1
        )

        self._assert_paths_equal(self._COPENHAGEN_TO_STOCKHOLM_PATH, path)

    def test_batch_interpolate(self):
        paths = self._INTERPOLATOR.batch_interpolate(
            [
                (self._COPENHAGEN_COORDINATES, self._SINGAPORE_COORDINATES),
                (self._COPENHAGEN_COORDINATES, self._STOCKHOLM_COORDINATES),
            ],
            a_star_greediness_weighting=1.1,
        )
        expected_paths = [
            self._COPENHAGEN_TO_SINGAPORE_PATH,
            self._COPENHAGEN_TO_STOCKHOLM_PATH,
        ]

        for expected_path, path in zip(expected_paths, paths):
            self._assert_paths_equal(expected_path, path)

    def test_cross_meridian(self):
        coords_1 = Coordinate(latitude=1, longitude=104)
        coords_2 = Coordinate(latitude=37, longitude=-125)

        expected_path = [
            TestPathInterpolator._make_coordinate(104.0, 1.00),
            TestPathInterpolator._make_coordinate(122.471194, 10.478333),
            TestPathInterpolator._make_coordinate(123.194167, 11.004527999999999),
            TestPathInterpolator._make_coordinate(124.311611, 11.565389),
            TestPathInterpolator._make_coordinate(124.97874999999999, 11.399167),
            TestPathInterpolator._make_coordinate(125.008389, 11.304532),
            TestPathInterpolator._make_coordinate(125.75874999999999, 11.019139),
            TestPathInterpolator._make_coordinate(-125.0, 37.0),
        ]

        path = self._INTERPOLATOR.interpolate(coords_1, coords_2, a_star_greediness_weighting=1.1)

        self._assert_paths_equal(expected_path, path)

    def test_non_intersecting(self):
        coords_1 = TestPathInterpolator._make_coordinate(-79.80740, 9.13017)
        coords_2 = TestPathInterpolator._make_coordinate(-79.81564, 9.14596)

        expected_path = [coords_1, coords_2]

        path = self._INTERPOLATOR.interpolate(coords_1, coords_2, a_star_greediness_weighting=1.1)

        self._assert_paths_equal(expected_path, path)

    @staticmethod
    def _make_coordinate(longitude: float, latitude: float) -> Coordinate:
        return Coordinate(
            latitude=latitude,
            longitude=longitude,
        )

    @staticmethod
    def _assert_paths_equal(
        expected_path: typing.Sequence[Coordinate],
        actual_path: typing.Sequence[Coordinate],
    ) -> None:
        dist = TestPathInterpolator._path_difference(expected_path, actual_path)
        tolerance = 240

        if dist > tolerance:
            expected_path_len = TestPathInterpolator._path_length(expected_path)
            actual_path_len = TestPathInterpolator._path_length(actual_path)

            raise AssertionError(
                f"Paths do not equal within tolerance of {tolerance}. "
                f"Expected path len: {expected_path_len}. "
                f"Actual path len: {actual_path_len}. "
            )

    @staticmethod
    def _path_length(path: typing.Sequence[Coordinate]) -> float:
        length = 0
        for i in range(1, len(path)):
            length += haversine(
                (path[i - 1].latitude, path[i - 1].longitude),
                (path[i].latitude, path[i].longitude),
            )
        return length

    @staticmethod
    def _path_difference(path1: typing.Sequence[Coordinate], path2: typing.Sequence[Coordinate]) -> float:
        if len(path1) < len(path2):
            s_path = path1
            l_path = path2
        else:
            s_path = path2
            l_path = path1

        path_diff_sum = 0.0
        for l_path_pt in l_path:
            shortest_dist_to_l_path = float("inf")
            for s_path_pt in s_path:
                shortest_dist_to_l_path = min(
                    shortest_dist_to_l_path,
                    haversine((l_path_pt.latitude, l_path_pt.longitude), (s_path_pt.latitude, s_path_pt.longitude)),
                )
            path_diff_sum += shortest_dist_to_l_path
        return path_diff_sum / len(s_path)
