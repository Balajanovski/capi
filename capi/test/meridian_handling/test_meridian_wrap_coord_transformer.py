import typing
import unittest

import hypothesis
import hypothesis.strategies as st

from capi.src.dtos.coordinate import Coordinate
from capi.src.hypothesis_strategies.coordinate import coordinate_strategy
from capi.src.meridian_handling.meridian_wrap_coord_transformer import MeridianWrapCoordTransformer


class TestMeridianWrapCoordTransformer(unittest.TestCase):
    def test_wrap_coords_alters_nothing_when_not_spanning_half_globe(self):
        coords = [
            Coordinate(longitude=-90, latitude=1),
            Coordinate(longitude=89.99, latitude=1),
            Coordinate(longitude=89.99, latitude=-1),
            Coordinate(longitude=-90, latitude=-1),
        ]
        transformer = MeridianWrapCoordTransformer()

        self.assertEqual(coords, transformer.wrap_coordinates(coords))

    def test_wrap_coords_wraps_when_spanning_half_globe(self):
        coords = [
            Coordinate(longitude=-90, latitude=1),
            Coordinate(longitude=90, latitude=1),
            Coordinate(longitude=90, latitude=-1),
            Coordinate(longitude=-90, latitude=-1),
        ]
        expected_coords = [
            Coordinate(longitude=270, latitude=1),
            Coordinate(longitude=90, latitude=1),
            Coordinate(longitude=90, latitude=-1),
            Coordinate(longitude=270, latitude=-1),
        ]

        transformer = MeridianWrapCoordTransformer()

        self.assertEqual(expected_coords, transformer.wrap_coordinates(coords))

    def test_wrap_coords_empty_list(self):
        self.assertEqual([], MeridianWrapCoordTransformer().wrap_coordinates([]))

    @hypothesis.given(st.lists(coordinate_strategy()))
    def test_reverse_wrap_is_inverse_of_wrap(self, coordinates: typing.Sequence[Coordinate]):
        transformer = MeridianWrapCoordTransformer()

        wrapped = transformer.wrap_coordinates(coordinates)
        reverse_wrapped = transformer.reverse_wrap(wrapped)

        for expected_coord, actual_coord in zip(coordinates, reverse_wrapped):
            self.assertAlmostEqual(expected_coord.longitude, actual_coord.longitude, delta=0.001)
            self.assertAlmostEqual(expected_coord.latitude, actual_coord.latitude, delta=0.001)
