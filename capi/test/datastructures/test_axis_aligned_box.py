import unittest

from capi.src.implementation.datastructures.axis_aligned_box import AxisAlignedBox
from capi.src.implementation.dtos.coordinate import Coordinate


class TestAxisAlignedBox(unittest.TestCase):
    def test_construct_normal(self):
        vertices = [
            Coordinate(longitude=-1, latitude=1),
            Coordinate(longitude=1, latitude=1),
            Coordinate(longitude=1, latitude=-1),
            Coordinate(longitude=-1, latitude=-1),
        ]

        box = AxisAlignedBox(vertices[0], vertices[2])

        self.assertEqual(vertices, box.vertices)
        self.assertEqual(vertices[0], box.upper_left)
        self.assertEqual(vertices[1], box.upper_right)
        self.assertEqual(vertices[2], box.lower_right)
        self.assertEqual(vertices[3], box.lower_left)

    def test_is_point_inside(self):
        box = AxisAlignedBox(Coordinate(longitude=-1, latitude=1), Coordinate(longitude=1, latitude=-1))

        # Interior points
        self.assertTrue(box.is_point_inside(Coordinate(longitude=0, latitude=0)))
        self.assertTrue(box.is_point_inside(Coordinate(longitude=-0.99, latitude=-0.99)))

        # Boundary points
        self.assertFalse(box.is_point_inside(Coordinate(longitude=1, latitude=0)))

        # Exterior points
        self.assertFalse(box.is_point_inside(Coordinate(longitude=20, latitude=20)))

    def test_is_point_inside_meridian_spanning(self):
        box = AxisAlignedBox(Coordinate(longitude=179, latitude=1), Coordinate(longitude=-179, latitude=-1))

        # Interior points
        self.assertTrue(box.is_point_inside(Coordinate(longitude=180, latitude=0)))
        self.assertTrue(box.is_point_inside(Coordinate(longitude=-180, latitude=0)))
        self.assertTrue(box.is_point_inside(Coordinate(longitude=179.1, latitude=-0.99)))

        # Boundary points
        self.assertFalse(box.is_point_inside(Coordinate(longitude=-179, latitude=0)))

        # Exterior points
        self.assertFalse(box.is_point_inside(Coordinate(longitude=-150, latitude=20)))
