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
from __future__ import division

import unittest
from math import cos, degrees, pi, sin

import capi.src.implementation.pyvisgraph as vg
from capi.src.implementation.pyvisgraph.geometry import intersect_point, point_in_polygon
from capi.src.implementation.pyvisgraph.graph import Edge, Graph, Point
from capi.src.implementation.pyvisgraph.visible_vertices import (
    angle_arctan,
    edge_intersect,
    euclidean_distance,
    point_edge_distance,
    visible_vertices,
)


class TestUndirectedGraph(unittest.TestCase):
    def setUp(self) -> None:
        self.point_a = Point(0.0, 1.0)
        self.point_b = Point(1.0, 2.0)
        self.point_c = Point(0.0, 1.0)
        self.point_d = Point(1.0, 2.0)
        self.edge_a = Edge(self.point_a, self.point_b)
        self.edge_b = Edge(self.point_b, self.point_a)
        self.edge_c = Edge(self.point_c, self.point_d)
        self.edge_d = Edge(self.point_d, self.point_c)

    def test_point_equality(self):
        assert self.point_a == Point(0.0, 1.0)
        assert self.point_a != Point(1.0, 0.0)
        assert self.edge_a == self.edge_b
        assert self.edge_a == self.edge_a
        assert self.edge_a == self.edge_c
        assert self.edge_a == self.edge_d


class TestAngleFunction(unittest.TestCase):
    @staticmethod
    def test_angle_function():
        center = Point(1.0, 1.0)
        point_a = Point(3.0, 1.0)
        point_b = Point(1.0, 0)
        point_c = Point(0.0, 2.0)
        point_d = Point(2.0, 2.0)
        point_e = Point(2.0, 0.0)
        point_f = Point(0.0, 0.0)
        assert angle_arctan(center, point_a) == 0
        assert angle_arctan(center, point_b) == pi * 3 / 2
        assert degrees(angle_arctan(center, point_c)) == 135
        assert degrees(angle_arctan(center, point_d)) == 45
        assert degrees(angle_arctan(center, point_e)) == 315
        assert degrees(angle_arctan(center, point_f)) == 225

    @staticmethod
    def test_angle_function_floating_point_errors():
        polys = [
            [
                Point(353.6790486272709, 400.99387840984855),
                Point(351.1303807396073, 398.8696192603927),
                Point(349.5795890382704, 397.8537806679034),
                Point(957.1067811865476, -207.10678118654744),
                Point(-457.10678118654766, -207.10678118654744),
                Point(-457.10678118654744, 1207.1067811865476),
                Point(957.1067811865476, 1207.1067811865473),
                Point(353.52994294901674, 606.0798841165788),
                Point(354.0988628008279, 604.098862800828),
                Point(354.52550331744527, 601.3462324760635),
                Point(352.6969055662087, 602.6943209889012),
                Point(351.22198101804634, 603.781672670995),
                Point(247.0, 500.0),
                Point(341.8964635104416, 405.50444716676054),
                Point(349.24224903733045, 410.671256247085),
                Point(350.84395848060774, 407.17766877398697),
            ]
        ]
        v = vg.VisGraph()
        v.build(polys)


class TestEdgeIntersectFunction(unittest.TestCase):
    @staticmethod
    def test_edge_intersect_function():
        point_a = Point(3.0, 5.0)
        point_b = Point(5.0, 3.0)
        point_c = Point(4.0, 2.0)
        point_d = Point(4.0, 5.0)
        point_e = Point(5.0, 4.0)
        point_f = Point(3.0, 4.0)
        point_g = Point(4.0, 1.0)
        point_h = Point(6.0, 4.0)
        point_i = Point(4.0, 4.0)
        edge = Edge(point_a, point_b)
        assert edge_intersect(point_c, point_d, edge) is True
        assert edge_intersect(point_c, point_e, edge) is True
        assert edge_intersect(point_f, point_e, edge) is True
        assert edge_intersect(point_g, point_b, edge) is True
        assert edge_intersect(point_c, point_h, edge) is True
        assert edge_intersect(point_h, point_i, edge) is True


class TestPointEdgeDistanceFunction(unittest.TestCase):
    @staticmethod
    def test_point_edge_distance_function():
        point_a = Point(3.0, 1.0)
        point_b = Point(3.0, 5.0)
        point_c = Point(2.0, 2.0)
        point_d = Point(4.0, 4.0)
        point_e = Point(1.0, 1.0)
        point_f = Point(1.0, 2.0)
        point_g = Point(3.0, 4.0)
        point_h = Point(2.0, 5.0)
        edge = Edge(point_a, point_b)
        edge2 = Edge(point_c, point_d)
        edge3 = Edge(point_e, point_b)
        assert point_edge_distance(point_c, point_d, edge) == 1.4142135623730951
        assert point_edge_distance(point_a, point_b, edge2) == 2.0
        assert point_edge_distance(point_f, point_g, edge3) == 1.4142135623730951
        assert point_edge_distance(point_h, point_g, edge3) == 0.9428090415820635


class TestPointInPolygon(unittest.TestCase):
    @staticmethod
    def test_point_in_polygon():
        graph = vg.VisGraph()
        point_a = Point(0, 0)
        point_b = Point(4, 0)
        point_c = Point(2, 4)
        point_d = Point(1, 0.5)
        graph.build([[point_a, point_b, point_c]])
        assert graph.point_in_polygon(point_d) != -1


class TestClosestPoint(unittest.TestCase):
    def setUp(self) -> None:
        self.graph = vg.VisGraph()
        self.point_a = Point(0, 0)
        self.point_b = Point(4, 0)
        self.point_c = Point(2, 4)
        self.point_d = Point(1, 0.5)
        self.graph.build([[self.point_a, self.point_b, self.point_c]])

    def test_closest_point(self):
        polygon_id = self.graph.point_in_polygon(self.point_d)
        closest_point = self.graph.closest_point(self.point_d, polygon_id)
        assert self.graph.point_in_polygon(closest_point) == -1

    def test_closest_point_length(self):
        polygon_id = self.graph.point_in_polygon(self.point_d)
        closest_point = self.graph.closest_point(self.point_d, polygon_id, length=0.5)
        intersection_point = intersect_point(self.point_d, closest_point, Edge(self.point_a, self.point_b))
        assert euclidean_distance(intersection_point, closest_point) == 0.5

    @staticmethod
    def test_closest_point_edge_point():
        """Test where the closest point is a end-point of a polygon edge. Can end up with
        cp extending into polygon instead of outside it."""
        graph = vg.VisGraph()
        graph.build([[Point(0, 1), Point(2, 0), Point(1, 1), Point(2, 2)]])
        point = Point(1, 0.9)
        polygon_id = graph.point_in_polygon(point)
        closest_point = graph.closest_point(point, polygon_id, length=0.001)
        assert graph.point_in_polygon(closest_point) == -1


class TestCollinear(unittest.TestCase):
    def setUp(self) -> None:
        self.point_a = Point(0.0, 1.0)
        self.point_b = Point(1.0, 0.0)
        self.point_c = Point(2.0, 3.0)
        self.point_d = Point(3.0, 2.0)
        self.point_e = Point(3.5, 0.5)
        self.point_f = Point(4.5, 3.5)

    def test_collin1(self):
        graph = Graph(
            [
                [self.point_a, self.point_b, self.point_c],
                [self.point_d, self.point_e, self.point_f],
            ]
        )
        visible = visible_vertices(Point(1, 4), graph, None, None)
        assert visible == [self.point_a, self.point_c, self.point_d, self.point_f]

    def test_collin2(self):
        self.point_g = Point(2.0, 5.0)
        self.point_h = Point(3.0, 5.0)
        graph = Graph(
            [
                [self.point_g, self.point_h, self.point_c],
                [self.point_d, self.point_e, self.point_f],
            ]
        )
        visible = visible_vertices(Point(1, 4), graph, None, None)
        assert visible == [self.point_g, self.point_e, self.point_c, self.point_d]

    def test_collin3(self):
        point_g = Point(2.0, 2.0)
        point_h = Point(3.5, 5.0)
        point_i = Point(2.5, 2.0)
        graph = Graph(
            [
                [self.point_a, self.point_b, self.point_c],
                [point_g, point_h, point_i],
                [self.point_d, self.point_e, self.point_f],
            ]
        )
        visible = visible_vertices(Point(1, 4), graph, None, None)
        assert visible == [point_h, self.point_a, self.point_c]

    @staticmethod
    def test_collin4():
        graph = Graph([[Point(1, 1), Point(2, 3), Point(3, 1), Point(2, 2)], [Point(2, 4)]])
        visible = visible_vertices(Point(2, 1), graph, None, None)
        assert visible == [Point(3, 1), Point(2, 2), Point(1, 1)]

    @staticmethod
    def test_collin5():
        polygon_radius = 0.2
        num_polygon_sides = 4
        polygon_center = Point(1.0, 1.0)
        verts = []
        for i in range(num_polygon_sides):
            verts.append(
                Point(
                    polygon_radius * cos(2 * pi * i / num_polygon_sides - pi / 4) + polygon_center.x,
                    polygon_radius * sin(2 * pi * i / num_polygon_sides - pi / 4) + polygon_center.y,
                )
            )
        graph = vg.VisGraph()
        graph.build([verts])
        point_1 = Point(0, 0)
        point_2 = Point(1.7, 1.7)
        shortest = graph.shortest_path(point_1, point_2)
        visible = visible_vertices(point_2, graph.graph, point_1, None)
        assert verts[3] not in visible
        assert verts[1] not in shortest
        assert verts[3] not in shortest

    # See https://github.com/TaipanRex/pyvisgraph/issues/20.
    # This tests colinearity case #1 using point_in_polygon.
    @staticmethod
    def test_collin6():
        graph = Graph([[Point(0, 0), Point(2, 1), Point(0, 2)]])
        pip = point_in_polygon(Point(1, 1), graph)
        assert pip > -1

    # See https://github.com/TaipanRex/pyvisgraph/issues/20.
    # This tests colinearity case #2 using point_in_polygon.
    @staticmethod
    def test_collin7():
        graph = Graph([[Point(0, 0), Point(1, 1), Point(2, 0), Point(2, 2), Point(0, 2)]])
        pip = point_in_polygon(Point(0.5, 1), graph)
        assert pip > -1

    # See https://github.com/TaipanRex/pyvisgraph/issues/20.
    # This tests colinearity case #3 using point_in_polygon.
    @staticmethod
    def test_collin8():
        graph = Graph([[Point(0, 0), Point(2, 0), Point(2, 2), Point(1, 1), Point(0, 2)]])
        pip = point_in_polygon(Point(0.5, 1), graph)
        assert pip > -1

    # See https://github.com/TaipanRex/pyvisgraph/issues/20.
    # This tests colinearity case #4 using point_in_polygon.
    @staticmethod
    def test_collin9():
        graph = Graph(
            [
                [
                    Point(0, 0),
                    Point(1, 0),
                    Point(1, 1),
                    Point(2, 1),
                    Point(2, 2),
                    Point(0, 2),
                ]
            ]
        )
        pip = point_in_polygon(Point(0.5, 1), graph)
        assert pip > -1

    # See https://github.com/TaipanRex/pyvisgraph/issues/20.
    # This tests colinearity case #5 using point_in_polygon.
    @staticmethod
    def test_collin10():
        graph = Graph(
            [
                [
                    Point(0, 0),
                    Point(1, 0),
                    Point(1, 1),
                    Point(2, 1),
                    Point(2, 0),
                    Point(3, 0),
                    Point(3, 2),
                    Point(0, 2),
                ]
            ]
        )
        pip = point_in_polygon(Point(0.5, 1), graph)
        assert pip > -1

    # See https://github.com/TaipanRex/pyvisgraph/issues/20.
    # This tests colinearity case #6 using point_in_polygon.
    @staticmethod
    def test_collin11():
        graph = Graph(
            [
                [
                    Point(0, 0),
                    Point(3, 0),
                    Point(3, 2),
                    Point(2, 3),
                    Point(2, 1),
                    Point(1, 1),
                    Point(1, 2),
                    Point(0, 2),
                ]
            ]
        )
        pip = point_in_polygon(Point(0.5, 1), graph)
        assert pip > -1
