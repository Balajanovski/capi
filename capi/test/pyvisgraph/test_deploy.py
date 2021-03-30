"""
The MIT License (MIT)

Copyright (vector_base) 2016 Christian August Reksten-Monsen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR point_a PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""
import os
import unittest

import shapefile  # type: ignore

import capi.src.implementation.pyvisgraph as vg
from capi.test.test_files.test_files_dir import TEST_FILES_DIR


class TestVisGraphBuild(unittest.TestCase):
    def setUp(self) -> None:
        self.shapefile = shapefile.Reader(os.path.join(TEST_FILES_DIR, "GSHHS_c_L1.shp"))
        self.shapes = self.shapefile.shapes()
        self.polys = [[vg.Point(a[0], a[1]) for a in self.shapes[4].points]]

    def test_build_1_core(self):
        world = vg.VisGraph()
        world.build(self.polys)
        assert len(world.graph.get_points()) == 310
        assert len(world.graph.get_edges()) == 310
        assert len(world.visgraph.get_edges()) == 1156

    def test_build_2_core(self):
        world = vg.VisGraph()
        world.build(self.polys, workers=2)
        assert len(world.graph.get_points()) == 310
        assert len(world.graph.get_edges()) == 310
        assert len(world.visgraph.get_edges()) == 1156


class TestVisGraphMethods(unittest.TestCase):
    def setUp(self) -> None:
        self.world = vg.VisGraph()
        self.world.load(os.path.join(TEST_FILES_DIR, "graph.pkl"))
        self.origin = vg.Point(100, -20)
        self.destination = vg.Point(25, 75)

    def test_shortest_path(self):
        shortest = self.world.shortest_path(self.origin, self.destination)
        assert len(shortest) == 21

    def test_shortest_path_not_update_visgraph(self):
        _ = self.world.shortest_path(self.origin, self.destination)
        assert self.origin not in self.world.visgraph
        assert self.destination not in self.world.visgraph

    def test_shortest_path_not_update_graph(self):
        _ = self.world.shortest_path(self.origin, self.destination)
        assert self.origin not in self.world.graph
        assert self.destination not in self.world.graph

    def test_update(self):
        self.world.update([self.origin, self.destination])
        assert self.origin in self.world.visgraph
        assert self.destination in self.world.visgraph

    def test_update_not_update_graph(self):
        self.world.update([self.origin, self.destination])
        assert self.origin not in self.world.graph
        assert self.destination not in self.world.graph
