import os
import unittest
from tempfile import TemporaryDirectory

from capi.src.implementation.graph_generator import GraphGenerator
from capi.src.implementation.visibility_graphs import load_graph_from_file
from capi.test.test_files.test_files_dir import TEST_FILES_DIR


class TestGraphGenerator(unittest.TestCase):
    def test_generate_non_meridian_spanning(self):
        expected_graph_path = os.path.join(TEST_FILES_DIR, "smaller_graph.pkl")

        with TemporaryDirectory() as temp_dir:
            output_graph_path = os.path.join(temp_dir, "out_smaller_graph.pkl")

            expected_graph = load_graph_from_file(expected_graph_path)

            generator = GraphGenerator()
            generator.generate(
                os.path.join(TEST_FILES_DIR, "smaller.shp"),
                output_graph_path,
                meridian_crossing=False,
            )

            actual_graph = load_graph_from_file(output_graph_path)

        self.assertEqual(expected_graph, actual_graph)

    def test_generate_meridian_spanning(self):
        expected_graph_path = os.path.join(TEST_FILES_DIR, "smaller_meridian_graph.pkl")

        with TemporaryDirectory() as temp_dir:
            output_graph_path = os.path.join(temp_dir, "out_meridian_smaller_graph.pkl")

            expected_graph = load_graph_from_file(expected_graph_path)

            generator = GraphGenerator()
            generator.generate(
                os.path.join(TEST_FILES_DIR, "smaller.shp"),
                output_graph_path,
                meridian_crossing=True,
            )

            actual_graph = load_graph_from_file(output_graph_path)

        self.assertEqual(expected_graph, actual_graph)
