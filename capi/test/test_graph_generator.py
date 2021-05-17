import os
import unittest
from tempfile import TemporaryDirectory

from capi.src.implementation.graph_generator import GraphGenerator
from capi.src.implementation.visibility_graphs import load_graph_from_file
from capi.test.test_files.test_files_dir import TEST_FILES_DIR
from capi.src.implementation.datastructures.graph_file_paths import GraphFilePaths


class TestGraphGenerator(unittest.TestCase):
    def test_generate(self):
        expected_graph_path = os.path.join(TEST_FILES_DIR, "smaller_graph")

        with TemporaryDirectory() as temp_dir:
            output_graph_path = os.path.join(temp_dir, "out_smaller_graph")

            expected_graph_paths = GraphFilePaths(expected_graph_path)
            expected_normal_graph = load_graph_from_file(expected_graph_paths.default_graph_path)
            expected_meridian_spanning_graph = load_graph_from_file(expected_graph_paths.meridian_graph_path)

            generator = GraphGenerator()
            generator.generate(
                os.path.join(TEST_FILES_DIR, "smaller.shp"),
                output_graph_path,
            )

            actual_graph_paths = GraphFilePaths(output_graph_path)
            actual_normal_graph = load_graph_from_file(actual_graph_paths.default_graph_path)
            actual_meridian_spanning_graph = load_graph_from_file(actual_graph_paths.meridian_graph_path)

        self.assertEqual(expected_normal_graph, actual_normal_graph)
        self.assertEqual(expected_meridian_spanning_graph, actual_meridian_spanning_graph)

    def test_generate_for_vertex_range(self):
        for test_case in [(0, 2, "smaller_graph_range_1"), (1, 2, "smaller_graph_range_2")]:
            expected_graph_path = os.path.join(TEST_FILES_DIR, test_case[2])

            with TemporaryDirectory() as temp_dir:
                output_graph_path = os.path.join(temp_dir, f"out_{test_case[2]}")

                expected_graph_paths = GraphFilePaths(expected_graph_path)
                expected_normal_graph = load_graph_from_file(expected_graph_paths.default_graph_path)
                expected_meridian_spanning_graph = load_graph_from_file(expected_graph_paths.meridian_graph_path)

                generator = GraphGenerator()
                generator.generate_for_vertex_range(
                    os.path.join(TEST_FILES_DIR, "smaller.shp"),
                    output_graph_path,
                    test_case[0],
                    test_case[1],
                    42,
                )

                actual_graph_paths = GraphFilePaths(output_graph_path)
                actual_normal_graph = load_graph_from_file(actual_graph_paths.default_graph_path)
                actual_meridian_spanning_graph = load_graph_from_file(actual_graph_paths.meridian_graph_path)

            self.assertEqual(expected_normal_graph, actual_normal_graph)
            self.assertEqual(expected_meridian_spanning_graph, actual_meridian_spanning_graph)
