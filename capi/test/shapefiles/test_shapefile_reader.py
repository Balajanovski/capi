import os
import unittest

from capi.src.implementation.datastructures.polygon import Polygon
from capi.src.implementation.dtos.coordinate import Coordinate
from capi.src.implementation.shapefiles.shapefile_reader import ShapefileReader
from capi.test.test_files.test_files_dir import TEST_FILES_DIR


class TestShapefileReader(unittest.TestCase):
    def test_read(self):
        reader = ShapefileReader()
        shapefile_path = os.path.join(TEST_FILES_DIR, "very_small.shp")

        expected_polygons = [
            Polygon(
                [
                    Coordinate(longitude=-1.0, latitude=0.0),
                    Coordinate(longitude=0.0, latitude=1.0),
                    Coordinate(longitude=1.0, latitude=0.0),
                    Coordinate(longitude=0.0, latitude=-1.0),
                ]
            )
        ]

        polygons = reader.read(shapefile_path)
        self.assertEqual(expected_polygons, polygons)
