import unittest

from capi.src.datastructures.axis_aligned_box import AxisAlignedBox
from capi.src.datastructures.polygon import Polygon
from capi.src.datastructures.polygon_spatial_index import PolygonSpatialIndex
from capi.src.dtos.coordinate import Coordinate


class TestPolygonSpatialIndex(unittest.TestCase):
    def test_intersecting_polygons_with_box(self):
        polygons = [
            Polygon(
                vertices=[
                    Coordinate(longitude=-1, latitude=0),
                    Coordinate(longitude=-2, latitude=-1),
                    Coordinate(longitude=-2, latitude=1),
                ]
            ),
            Polygon(
                vertices=[
                    Coordinate(longitude=1, latitude=1),
                    Coordinate(longitude=2, latitude=1),
                    Coordinate(longitude=2, latitude=-1),
                    Coordinate(longitude=1, latitude=-1),
                ]
            ),
        ]

        index = PolygonSpatialIndex(polygons=polygons)

        query_1 = AxisAlignedBox(
            top_left=Coordinate(longitude=0, latitude=1), bottom_right=Coordinate(longitude=1, latitude=-1)
        )
        query_2 = AxisAlignedBox(
            top_left=Coordinate(longitude=-1, latitude=1), bottom_right=Coordinate(longitude=0, latitude=-1)
        )
        query_3 = AxisAlignedBox(
            top_left=Coordinate(longitude=-2.5, latitude=0.5), bottom_right=Coordinate(longitude=2.5, latitude=0.5)
        )
        query_4 = AxisAlignedBox(
            top_left=Coordinate(longitude=-0.5, latitude=0.5), bottom_right=Coordinate(longitude=0.5, latitude=-0.5)
        )

        query_res_1 = index.intersecting_polygons_with_box(query_1)
        query_res_2 = index.intersecting_polygons_with_box(query_2)
        query_res_3 = index.intersecting_polygons_with_box(query_3)
        query_res_4 = index.intersecting_polygons_with_box(query_4)

        self.assertEqual([polygons[1]], query_res_1)
        self.assertEqual([polygons[0]], query_res_2)
        self.assertEqual(polygons, query_res_3)
        self.assertEqual([], query_res_4)
