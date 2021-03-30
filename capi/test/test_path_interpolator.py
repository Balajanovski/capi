import os
import typing
import unittest

from haversine import haversine  # type: ignore

from capi.src.implementation.dtos.coordinate import Coordinate
from capi.src.implementation.path_interpolator import PathInterpolator
from capi.test.test_files.test_files_dir import TEST_FILES_DIR


class TestPathInterpolator(unittest.TestCase):
    _EPSILON_TOLERANCE = 0.01
    _SHAPEFILE_FILE_PATH = os.path.join(TEST_FILES_DIR, "GSHHS_c_L1.shp")

    def test_copenhagen_to_singapore(self):
        copenhagen_coordinates = Coordinate(latitude=55, longitude=13)
        singapore_coordinates = Coordinate(latitude=1, longitude=104)
        graph_file_path = os.path.join(TEST_FILES_DIR, "graph.pkl")

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
            visibility_graph_file_path=graph_file_path, shapefile_file_path=self._SHAPEFILE_FILE_PATH
        )
        path = interpolator.interpolate(copenhagen_coordinates, singapore_coordinates)

        self._assert_paths_equal(expected_path, path, self._EPSILON_TOLERANCE)

    def test_cross_meridian_with_meridian_graph(self):
        coords_1 = Coordinate(latitude=1, longitude=104)
        coords_2 = Coordinate(latitude=37, longitude=-125)
        graph_file_path = os.path.join(TEST_FILES_DIR, "graph.pkl")
        meridian_graph_file_path = os.path.join(TEST_FILES_DIR, "meridian_graph.pkl")

        expected_path = [
            self._make_coordinate(104, 1.00),
            self._make_coordinate(116.74586, 7.034528),
            self._make_coordinate(123.012472, 9.040389),
            self._make_coordinate(123.451306, 9.194139),
            self._make_coordinate(126.06253, 10.0621),
            self._make_coordinate(-125.0, 37.0),
        ]

        interpolator = PathInterpolator(
            visibility_graph_file_path=graph_file_path,
            meridian_crossing_visibility_graph_file_path=meridian_graph_file_path,
            shapefile_file_path=self._SHAPEFILE_FILE_PATH,
        )
        path = interpolator.interpolate(coords_1, coords_2)

        self._assert_paths_equal(expected_path, path, self._EPSILON_TOLERANCE)

    @staticmethod
    def _get_path_length(path: typing.Sequence[Coordinate]) -> float:
        length = 0
        for i in range(1, len(path)):
            length += haversine((path[i - 1].latitude, path[i - 1].longitude), (path[i].latitude, path[i].longitude))
        return length

    def test_cross_meridian_without_meridian_graph(self):
        coords_1 = Coordinate(latitude=1, longitude=104)
        coords_2 = Coordinate(latitude=37, longitude=-125)
        graph_file_path = os.path.join(TEST_FILES_DIR, "graph.pkl")

        expected_path = [
            self._make_coordinate(104.00, 1.00),
            self._make_coordinate(103.97, 1.00),
            self._make_coordinate(103.51, 1.27),
            self._make_coordinate(97.50, 5.26),
            self._make_coordinate(95.23, 5.58),
            self._make_coordinate(80.26, 6.00),
            self._make_coordinate(50.80, 11.99),
            self._make_coordinate(43.46, 12.67),
            self._make_coordinate(32.57, 29.93),
            self._make_coordinate(32.28, 31.23),
            self._make_coordinate(31.92, 31.53),
            self._make_coordinate(11.04, 37.09),
            self._make_coordinate(9.74, 37.35),
            self._make_coordinate(3.90, 36.93),
            self._make_coordinate(-5.47, 35.92),
            self._make_coordinate(-5.93, 35.79),
            self._make_coordinate(-61.60, 16.00),
            self._make_coordinate(-71.67, 12.46),
            self._make_coordinate(-79.62, 9.62),
            self._make_coordinate(-79.91, 9.37),
            self._make_coordinate(-79.55, 8.94),
            self._make_coordinate(-79.99, 7.52),
            self._make_coordinate(-80.44, 7.24),
            self._make_coordinate(-80.89, 7.21),
            self._make_coordinate(-81.72, 7.56),
            self._make_coordinate(-82.89, 8.03),
            self._make_coordinate(-103.51, 18.31),
            self._make_coordinate(-110.01, 22.89),
            self._make_coordinate(-112.18, 24.81),
            self._make_coordinate(-125.00, 37.00),
        ]

        interpolator = PathInterpolator(
            visibility_graph_file_path=graph_file_path,
            shapefile_file_path=self._SHAPEFILE_FILE_PATH,
        )
        path = interpolator.interpolate(coords_1, coords_2)

        self._assert_paths_equal(expected_path, path, self._EPSILON_TOLERANCE)

    def test_non_intersecting(self):
        coords_1 = self._make_coordinate(-79.80740, 9.13017)
        coords_2 = self._make_coordinate(-79.81564, 9.14596)
        graph_file_path = os.path.join(TEST_FILES_DIR, "graph.pkl")

        expected_path = [coords_1, coords_2]

        interpolator = PathInterpolator(
            visibility_graph_file_path=graph_file_path,
            shapefile_file_path=self._SHAPEFILE_FILE_PATH,
        )
        path = interpolator.interpolate(coords_1, coords_2)

        self._assert_paths_equal(expected_path, path, self._EPSILON_TOLERANCE)

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
        epsilon_tolerance: float,
    ) -> None:
        for coord_1, coord_2 in zip(path_1, path_2):
            if (
                abs(coord_1.longitude - coord_2.longitude) > epsilon_tolerance
                or abs(coord_1.latitude - coord_2.latitude) > epsilon_tolerance
            ):
                raise AssertionError(f"Paths do not equal within epsilon tolerance of {epsilon_tolerance}")
