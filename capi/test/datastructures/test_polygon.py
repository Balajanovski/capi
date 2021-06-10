import typing
import unittest

import hypothesis
import hypothesis.strategies as st

from capi.src.hypothesis_strategies.coordinate import coordinate_strategy
from capi.src.hypothesis_strategies.polygon import polygon_strategy
from capi.src.implementation.datastructures.polygon import Polygon
from capi.src.implementation.dtos.coordinate import Coordinate
from capi.src.interfaces.datastructures.polygon import IPolygon


class TestPolygon(unittest.TestCase):
    @hypothesis.given(st.lists(coordinate_strategy(), min_size=3, max_size=5))
    def test_vertices(self, vertices: typing.Sequence[Coordinate]):
        poly = Polygon(vertices)
        self.assertEqual(vertices, poly.vertices)

    @hypothesis.given(st.lists(coordinate_strategy(), min_size=0, max_size=2))
    def test_construct_throws_error_when_less_than_3_vertices(self, vertices: typing.Sequence[Coordinate]):
        with self.assertRaises(ValueError):
            Polygon(vertices)
