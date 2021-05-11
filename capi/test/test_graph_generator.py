import os
import unittest
from tempfile import TemporaryDirectory

from capi.src.implementation.graph_generator import GraphGenerator
from capi.src.implementation.visibility_graphs import load_graph_from_file
from capi.test.test_files.test_files_dir import TEST_FILES_DIR


class TestGraphGenerator(unittest.TestCase):
    def test_generate(self):
        expected_graph_path = os.path.join(TEST_FILES_DIR, "smaller_graph")

        with TemporaryDirectory() as temp_dir:
            output_graph_path = os.path.join(temp_dir, "out_smaller_graph")

            expected_normal_graph = load_graph_from_file(os.path.join(expected_graph_path, "default"))
            expected_meridian_spanning_graph = load_graph_from_file(os.path.join(expected_graph_path, "meridian"))

            generator = GraphGenerator()
            generator.generate(
                os.path.join(TEST_FILES_DIR, "smaller.shp"),
                output_graph_path,
            )

            actual_normal_graph = load_graph_from_file(os.path.join(output_graph_path, "default"))
            actual_meridian_spanning_graph = load_graph_from_file(os.path.join(output_graph_path, "meridian"))

        self.assertEqual(expected_normal_graph, actual_normal_graph)
        self.assertEqual(expected_meridian_spanning_graph, actual_meridian_spanning_graph)
