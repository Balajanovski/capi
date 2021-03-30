import unittest

from capi.src.implementation.datastructures.polygon import Polygon
from capi.src.implementation.dtos.coordinate import Coordinate
from capi.src.implementation.datastructures.axis_aligned_box import AxisAlignedBox
from capi.src.implementation.polygon_clipper import PolygonClipper


class TestPolygonClipper(unittest.TestCase):
    _OCTAGON = Polygon(
        vertices=[
            Coordinate(longitude=1, latitude=2),
            Coordinate(longitude=2, latitude=1),
            Coordinate(longitude=2, latitude=-1),
            Coordinate(longitude=1, latitude=-2),
            Coordinate(longitude=-1, latitude=-2),
            Coordinate(longitude=-2, latitude=-1),
            Coordinate(longitude=-2, latitude=1),
            Coordinate(longitude=-1, latitude=2),
        ]
    )

    _CONCAVE_POLYGON = Polygon(
        vertices=[
            Coordinate(longitude=-2, latitude=0),
            Coordinate(longitude=2, latitude=1),
            Coordinate(longitude=1, latitude=0),
            Coordinate(longitude=2, latitude=-1),
        ]
    )

    _MERIDIAN_SPANNING_CONCAVE_POLYGON = Polygon(
        vertices=[
            Coordinate(longitude=178, latitude=0),
            Coordinate(longitude=-178, latitude=1),
            Coordinate(longitude=-179, latitude=0),
            Coordinate(longitude=-178, latitude=-1),
        ]
    )

    def test_clip_single_polygon_when_polygon_surrounding_box(self):
        box = AxisAlignedBox(
            top_left=Coordinate(longitude=-1, latitude=1), bottom_right=Coordinate(longitude=1, latitude=-1)
        )

        polygon = self._OCTAGON

        expected_clipped_polygon = Polygon(
            vertices=[
                Coordinate(longitude=1, latitude=1),
                Coordinate(longitude=1, latitude=-1),
                Coordinate(longitude=-1, latitude=-1),
                Coordinate(longitude=-1, latitude=1),
            ]
        )

        clipper = PolygonClipper(coord_merge_epsilon_tolerance=0.001)
        clipped_polygon = clipper.clip_single_polygon(polygon, box)

        self.assertEqual(expected_clipped_polygon, clipped_polygon)

    def test_clip_single_polygon_when_box_surrounding_polygon(self):
        box = AxisAlignedBox(
            top_left=Coordinate(longitude=-2, latitude=2),
            bottom_right=Coordinate(longitude=2, latitude=-2),
        )

        polygon = self._OCTAGON

        clipper = PolygonClipper()
        clipped_polygon = clipper.clip_single_polygon(polygon, box)

        self.assertEqual(polygon, clipped_polygon)

    def test_clip_single_polygon_quadrant(self):
        box = AxisAlignedBox(
            top_left=Coordinate(longitude=0, latitude=2),
            bottom_right=Coordinate(longitude=2, latitude=0),
        )

        polygon = self._OCTAGON

        clipper = PolygonClipper()
        clipped_polygon = clipper.clip_single_polygon(polygon, box)

        expected_clipped_polygon = Polygon(
            vertices=[
                Coordinate(longitude=1, latitude=2),
                Coordinate(longitude=2, latitude=1),
                Coordinate(longitude=2, latitude=0),
                Coordinate(longitude=0, latitude=0),
                Coordinate(longitude=0, latitude=2),
            ]
        )

        self.assertEqual(expected_clipped_polygon, clipped_polygon)

    def test_clip_single_polygon_half(self):
        box = AxisAlignedBox(
            top_left=Coordinate(longitude=0, latitude=2), bottom_right=Coordinate(longitude=2, latitude=-2)
        )

        polygon = self._OCTAGON

        clipper = PolygonClipper()
        clipped_polygon = clipper.clip_single_polygon(polygon, box)

        expected_clipped_polygon = Polygon(
            vertices=[
                Coordinate(longitude=1, latitude=2),
                Coordinate(longitude=2, latitude=1),
                Coordinate(longitude=2, latitude=-1),
                Coordinate(longitude=1, latitude=-2),
                Coordinate(longitude=0, latitude=-2),
                Coordinate(longitude=0, latitude=2),
            ]
        )

        self.assertEqual(expected_clipped_polygon, clipped_polygon)

    def test_clip_concave_polygon(self):
        box = AxisAlignedBox(
            top_left=Coordinate(longitude=-1, latitude=1), bottom_right=Coordinate(longitude=1, latitude=-1)
        )

        polygon = self._CONCAVE_POLYGON

        clipper = PolygonClipper()
        clipped_polygon = clipper.clip_single_polygon(polygon, box)

        expected_clipped_polygon = Polygon(
            vertices=[
                Coordinate(longitude=-1.0, latitude=0.25),
                Coordinate(longitude=1.0, latitude=0.75),
                Coordinate(longitude=1.0, latitude=-0.75),
                Coordinate(longitude=-1.0, latitude=-0.25),
            ]
        )

        self.assertEqual(expected_clipped_polygon, clipped_polygon)

    def test_clip_meridian_concave_polygon(self):
        box = AxisAlignedBox(
            top_left=Coordinate(longitude=179, latitude=1), bottom_right=Coordinate(longitude=-179, latitude=-1)
        )

        polygon = self._MERIDIAN_SPANNING_CONCAVE_POLYGON

        clipper = PolygonClipper()
        clipped_polygon = clipper.clip_single_polygon(polygon, box)

        expected_clipped_polygon = Polygon(
            vertices=[
                Coordinate(longitude=179.0, latitude=0.25),
                Coordinate(longitude=-179.0, latitude=0.75),
                Coordinate(longitude=-179.0, latitude=-0.75),
                Coordinate(longitude=179.0, latitude=-0.25),
            ]
        )

        self.assertEqual(expected_clipped_polygon, clipped_polygon)
