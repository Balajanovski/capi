import os
import unittest
from tempfile import TemporaryDirectory

from capi.src.implementation.datastructures.graph_file_paths import GraphFilePaths
from capi.src.implementation.graph_merger import GraphMerger
from capi.src.implementation.visibility_graphs import load_graph_from_file
from capi.test.test_files.test_files_dir import TEST_FILES_DIR


class TestGraphMerger(unittest.TestCase):
    def test_merge(self):
        smaller_graph_path = os.path.join(TEST_FILES_DIR, "smaller_graph")
        smaller_graph_range_1 = os.path.join(TEST_FILES_DIR, "smaller_graph_range_1")
        smaller_graph_range_2 = os.path.join(TEST_FILES_DIR, "smaller_graph_range_2")

        with TemporaryDirectory() as temp_dir:
            output_graph_path = os.path.join(temp_dir, "out_smaller_merged_graph")

            merger = GraphMerger()
            merger.merge([smaller_graph_range_1, smaller_graph_range_2], output_graph_path)

            expected_graph_file_paths = GraphFilePaths(smaller_graph_path)
            actual_graph_file_paths = GraphFilePaths(output_graph_path)

            expected_default_graph = load_graph_from_file(expected_graph_file_paths.default_graph_path)
            actual_default_graph = load_graph_from_file(actual_graph_file_paths.default_graph_path)

        self.assertEqual(expected_default_graph, actual_default_graph)
