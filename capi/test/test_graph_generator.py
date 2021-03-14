import os
import unittest
from tempfile import TemporaryDirectory

from capi.src.graph_generator import GraphGenerator
from capi.src.pyvisgraph.vis_graph import VisGraph
from capi.test.test_files.test_files_dir import TEST_FILES_DIR


class TestGraphGenerator(unittest.TestCase):
    def test_generate_non_meridian_spanning(self):
        expected_graph_path = os.path.join(TEST_FILES_DIR, "smaller_graph.pkl")

        with TemporaryDirectory() as temp_dir:
            output_graph_path = os.path.join(temp_dir, "out_smaller_graph.pkl")

            expected_graph = VisGraph()
            expected_graph.load(expected_graph_path)

            generator = GraphGenerator(num_workers=1)
            generator.generate(
                os.path.join(TEST_FILES_DIR, "smaller.shp"),
                output_graph_path,
                meridian_crossing=False,
            )

            actual_graph = VisGraph()
            actual_graph.load(output_graph_path)

        self.assertEqual(expected_graph, actual_graph)

    def test_generate_meridian_spanning(self):
        expected_graph_path = os.path.join(TEST_FILES_DIR, "smaller_meridian_graph.pkl")

        with TemporaryDirectory() as temp_dir:
            output_graph_path = os.path.join(temp_dir, "out_meridian_smaller_graph.pkl")

            expected_graph = VisGraph()
            expected_graph.load(expected_graph_path)

            generator = GraphGenerator(num_workers=1)
            generator.generate(
                os.path.join(TEST_FILES_DIR, "smaller.shp"),
                output_graph_path,
                meridian_crossing=True,
            )

            actual_graph = VisGraph()
            actual_graph.load(output_graph_path)

        self.assertEqual(expected_graph, actual_graph)
