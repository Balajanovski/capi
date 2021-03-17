import unittest

from capi.src.dtos.coordinate import Coordinate
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
