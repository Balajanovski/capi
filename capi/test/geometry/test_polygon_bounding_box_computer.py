import unittest

import hypothesis

from capi.src.datastructures.axis_aligned_box import AxisAlignedBox
from capi.src.datastructures.polygon import Polygon
from capi.src.dtos.coordinate import Coordinate
from capi.src.geometry.polygon_bounding_box_computer import PolygonBoundingBoxComputer
from capi.src.hypothesis_strategies.axis_aligned_box import axis_aligned_box_strategy


class TestPolygonBoundingBoxComputer(unittest.TestCase):
    def test_compute(self):
        polygon = Polygon(
            [
                Coordinate(longitude=-1, latitude=0),
                Coordinate(longitude=1, latitude=-1),
                Coordinate(longitude=0, latitude=1),
            ]
        )
        expected_box = AxisAlignedBox(
            Coordinate(longitude=-1, latitude=1),
            Coordinate(longitude=1, latitude=-1),
        )
        computer = PolygonBoundingBoxComputer()

        self.assertEqual(expected_box, computer.compute(polygon))

    @hypothesis.given(box=axis_aligned_box_strategy())
    def test_compute_on_axis_aligned_box_alters_nothing(self, box: AxisAlignedBox):
        computer = PolygonBoundingBoxComputer()
        self.assertEqual(box, computer.compute(box))
