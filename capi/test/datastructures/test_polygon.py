import typing
import unittest

import hypothesis
import hypothesis.strategies as st

from capi.interfaces.datastructures.polygon import IPolygon
from capi.src.datastructures.polygon import Polygon
from capi.src.dtos.coordinate import Coordinate
from capi.src.hypothesis_strategies.coordinate import coordinate_strategy
from capi.src.hypothesis_strategies.polygon import polygon_strategy


class TestPolygon(unittest.TestCase):
    @hypothesis.given(st.lists(coordinate_strategy(), min_size=3, max_size=5))
    def test_vertices(self, vertices: typing.Sequence[Coordinate]):
        poly = Polygon(vertices)
        self.assertEqual(vertices, poly.vertices)

    @hypothesis.given(st.lists(coordinate_strategy(), min_size=0, max_size=2))
    def test_construct_throws_error_when_less_than_3_vertices(self, vertices: typing.Sequence[Coordinate]):
        with self.assertRaises(ValueError):
            Polygon(vertices)

    def test_is_point_inside_regular(self):
        vertices = [
            Coordinate(longitude=-5, latitude=0),
            Coordinate(longitude=5, latitude=0),
            Coordinate(longitude=0, latitude=10),
        ]
        poly = Polygon(vertices)

        # Inside points
        self.assertTrue(poly.is_point_inside(Coordinate(longitude=0, latitude=2)))
        self.assertTrue(poly.is_point_inside(Coordinate(longitude=0, latitude=0.01)))
        self.assertTrue(poly.is_point_inside(Coordinate(longitude=-1, latitude=7.99)))
        self.assertTrue(poly.is_point_inside(Coordinate(longitude=2, latitude=5.99)))

        # Boundary points
        self.assertFalse(poly.is_point_inside(Coordinate(longitude=0, latitude=0)))
        self.assertFalse(poly.is_point_inside(Coordinate(longitude=-1, latitude=8)))
        self.assertFalse(poly.is_point_inside(Coordinate(longitude=2, latitude=6)))

        # Outside points
        self.assertFalse(poly.is_point_inside(Coordinate(longitude=-5, latitude=1)))
        self.assertFalse(poly.is_point_inside(Coordinate(longitude=5, latitude=1)))

    def test_is_point_inside_meridian_spanning(self):
        vertices = [
            Coordinate(longitude=175, latitude=0),
            Coordinate(longitude=-175, latitude=0),
            Coordinate(longitude=180, latitude=10),
        ]
        poly = Polygon(vertices)

        # Inside points
        self.assertTrue(poly.is_point_inside(Coordinate(longitude=180, latitude=2)))
        self.assertTrue(poly.is_point_inside(Coordinate(longitude=-180, latitude=2)))
        self.assertTrue(poly.is_point_inside(Coordinate(longitude=180, latitude=0.01)))
        self.assertTrue(poly.is_point_inside(Coordinate(longitude=179, latitude=7.99)))
        self.assertTrue(poly.is_point_inside(Coordinate(longitude=-178, latitude=5.99)))

        # Boundary points
        self.assertFalse(poly.is_point_inside(Coordinate(longitude=180, latitude=0)))
        self.assertFalse(poly.is_point_inside(Coordinate(longitude=-180, latitude=0)))
        self.assertFalse(poly.is_point_inside(Coordinate(longitude=179, latitude=8)))
        self.assertFalse(poly.is_point_inside(Coordinate(longitude=-178, latitude=6)))

        # Outside points
        self.assertFalse(poly.is_point_inside(Coordinate(longitude=175, latitude=1)))
        self.assertFalse(poly.is_point_inside(Coordinate(longitude=-175, latitude=1)))

    def test_line_intersects_vertices_clockwise(self):
        vertices = [
            Coordinate(longitude=-1, latitude=1),
            Coordinate(longitude=1, latitude=1),
            Coordinate(longitude=1, latitude=-1),
            Coordinate(longitude=-1, latitude=-1),
        ]
        poly = Polygon(vertices)

        self.assertTrue(
            poly.line_intersects(Coordinate(longitude=-1.25, latitude=1.5), Coordinate(longitude=0, latitude=0))
        )
        self.assertTrue(
            poly.line_intersects(Coordinate(longitude=0.25, latitude=0.25), Coordinate(longitude=-0.25, latitude=-0.25))
        )
        self.assertFalse(
            poly.line_intersects(Coordinate(longitude=-1.25, latitude=1.5), Coordinate(longitude=1.25, latitude=1.75))
        )

    def test_line_intersects_vertices_counterclockwise(self):
        vertices = [
            Coordinate(longitude=1, latitude=1),
            Coordinate(longitude=1, latitude=-1),
            Coordinate(longitude=-1, latitude=-1),
            Coordinate(longitude=-1, latitude=1),
        ]
        poly = Polygon(vertices)

        self.assertTrue(
            poly.line_intersects(Coordinate(longitude=-1.25, latitude=1.5), Coordinate(longitude=0, latitude=0))
        )
        self.assertTrue(
            poly.line_intersects(Coordinate(longitude=0.25, latitude=0.25), Coordinate(longitude=-0.25, latitude=-0.25))
        )
        self.assertFalse(
            poly.line_intersects(Coordinate(longitude=-1.25, latitude=1.5), Coordinate(longitude=1.25, latitude=1.75))
        )

    def test_line_intersects_vertices_meridian_spanning(self):
        vertices = [
            Coordinate(longitude=179, latitude=1),
            Coordinate(longitude=-179, latitude=1),
            Coordinate(longitude=-179, latitude=-1),
            Coordinate(longitude=179, latitude=-1),
        ]
        poly = Polygon(vertices)

        self.assertTrue(
            poly.line_intersects(Coordinate(longitude=178.75, latitude=1.5), Coordinate(longitude=180, latitude=0))
        )
        self.assertTrue(
            poly.line_intersects(
                Coordinate(longitude=-179.75, latitude=0.25), Coordinate(longitude=179.75, latitude=-0.25)
            )
        )
        self.assertFalse(
            poly.line_intersects(
                Coordinate(longitude=178.75, latitude=1.5), Coordinate(longitude=-178.75, latitude=1.75)
            )
        )

    @hypothesis.given(
        polygon=polygon_strategy(),
    )
    def test_to_shapely_polygon(self, polygon: IPolygon):
        shapely_poly = polygon.to_shapely_polygon()
        reconstructed_poly = Polygon.from_shapely_polygon(shapely_poly)

        self.assertEqual(polygon, reconstructed_poly)
