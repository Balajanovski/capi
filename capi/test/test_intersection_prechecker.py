import os
import unittest

from capi.src.dtos.coordinate import Coordinate
from capi.src.intersection_prechecker import IntersectionPrechecker
from capi.test.test_files.test_files_dir import TEST_FILES_DIR


class TestIntersectionPrechecker(unittest.TestCase):
    _intersection_prechecker = IntersectionPrechecker(
        shapefile_file_path=os.path.join(TEST_FILES_DIR, "GSHHS_h_L1.shp"),
    )

    def test_does_line_cross_polygons_no_cross(self):
        self.assertFalse(
            self._intersection_prechecker.does_line_cross_polygons(
                Coordinate(latitude=-0.005, longitude=0.093),
                Coordinate(latitude=0.016479, longitude=0.4111),
            )
        )

        # Great bitter lake in Suez canal
        self.assertFalse(
            self._intersection_prechecker.does_line_cross_polygons(
                Coordinate(latitude=30.36346, longitude=32.35464),
                Coordinate(latitude=30.31476, longitude=32.38598),
            )
        )

        # Gatun lake in Panama canal
        self.assertFalse(
            self._intersection_prechecker.does_line_cross_polygons(
                Coordinate(latitude=9.13017, longitude=-79.80740),
                Coordinate(latitude=9.14596, longitude=-79.81564),
            )
        )

    def test_does_line_cross_polygons_cross(self):
        # Minor Caribbean island: Zion's Hill
        self.assertTrue(
            self._intersection_prechecker.does_line_cross_polygons(
                Coordinate(latitude=17.62945, longitude=-63.27305),
                Coordinate(latitude=17.63219, longitude=-63.19361),
            )
        )
