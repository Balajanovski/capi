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
import typing
from collections import defaultdict


class Point:
    __slots__ = ("x", "y", "polygon_id")
    _EPSILON_TOLERANCE = 0.00000001

    def __init__(self, x: float, y: float, polygon_id: int = -1):
        self.x = x
        self.y = y
        self.polygon_id = polygon_id

    def __eq__(self, point: object) -> bool:
        if not isinstance(point, Point):
            return False

        return abs(self.x - point.x) < self._EPSILON_TOLERANCE and abs(self.y - point.y) < self._EPSILON_TOLERANCE

    def __ne__(self, point: object) -> bool:
        return not self.__eq__(point)

    def __lt__(self, point: object) -> bool:
        """This is only needed for shortest path calculations where heapq is
        used. When there are two points of equal distance, heapq will
        instead evaluate the Points, which doesnt work in Python 3 and
        throw a TypeError."""
        return hash(self) < hash(point)

    def __str__(self) -> str:
        return "(%.2f, %.2f)" % (self.x, self.y)

    def __hash__(self) -> int:
        return (hash(self.x) << 32) ^ hash(self.y)

    def __repr__(self) -> str:
        return "Point(%.2f, %.2f)" % (self.x, self.y)


class Edge:
    __slots__ = ("endpoint_1", "endpoint_2")

    def __init__(self, point1: Point, point2: Point):
        self.endpoint_1 = point1
        self.endpoint_2 = point2

    def get_adjacent(self, point: Point) -> Point:
        if point == self.endpoint_1:
            return self.endpoint_2
        return self.endpoint_1

    def __contains__(self, point: Point) -> bool:
        return point in (self.endpoint_1, self.endpoint_2)

    def __eq__(self, edge: object) -> bool:
        if not isinstance(edge, Edge):
            return False

        if self.endpoint_1 == edge.endpoint_1 and self.endpoint_2 == edge.endpoint_2:
            return True
        if self.endpoint_1 == edge.endpoint_2 and self.endpoint_2 == edge.endpoint_1:
            return True
        return False

    def __ne__(self, edge: object) -> bool:
        return not self.__eq__(edge)

    def __str__(self) -> str:
        return "({}, {})".format(self.endpoint_1, self.endpoint_2)

    def __repr__(self) -> str:
        return "Edge({!r}, {!r})".format(self.endpoint_1, self.endpoint_2)

    def __hash__(self) -> int:
        return (hash(self.endpoint_1) << 16) ^ hash(self.endpoint_2)


class Graph:
    """
    point_a Graph is represented by a dict where the keys are Points in the Graph
    and the dict values are sets containing Edges incident on each Point.
    point_a separate set *edges* contains all Edges in the graph.

    The polygons must be a list of polygons, where each polygon is a list of
    in-order (clockwise or counter clockwise) Points. If only one polygon,
    it must still be a list in a list, i.e. [[Point(0,0), Point(2,0),
    Point(2,1)]].

    *polygons* dictionary: key is a integer polygon ID and values are the
    edges that make up the polygon. Note only polygons with 3 or more Points
    will be classified as a polygon. Non-polygons like just one Point will be
    given a polygon ID of -1 and not maintained in the dict.
    """

    def __init__(self, polygons: typing.Sequence[typing.Sequence[Point]]):
        self.graph: typing.DefaultDict[Point, typing.Set[Edge]] = defaultdict(set)
        self.edges: typing.Set[Edge] = set()
        self.polygons: typing.DefaultDict[int, typing.Set[Edge]] = defaultdict(set)
        pid = 0

        for polygon in polygons:
            curr_polygon = list(polygon)

            if curr_polygon[0] == curr_polygon[-1] and len(curr_polygon) > 1:
                curr_polygon.pop()
            for i, point in enumerate(curr_polygon):
                sibling_point = curr_polygon[(i + 1) % len(curr_polygon)]
                edge = Edge(point, sibling_point)
                if len(curr_polygon) > 2:
                    point.polygon_id = pid
                    sibling_point.polygon_id = pid
                    self.polygons[pid].add(edge)
                self.add_edge(edge)
            if len(curr_polygon) > 2:
                pid += 1

    def get_adjacent_points(self, point: Point) -> typing.Sequence[Point]:
        return [edge.get_adjacent(point) for edge in self[point]]

    def get_points(self) -> typing.List[Point]:
        return list(self.graph)

    def get_edges(self) -> typing.Set[Edge]:
        return self.edges

    def add_edge(self, edge: Edge) -> None:
        self.graph[edge.endpoint_1].add(edge)
        self.graph[edge.endpoint_2].add(edge)
        self.edges.add(edge)

    def __contains__(self, item: typing.Union[Point, Edge]) -> bool:
        if isinstance(item, Point):
            return item in self.graph
        if isinstance(item, Edge):
            return item in self.edges
        return False

    def __getitem__(self, point: Point) -> typing.Set[Edge]:
        if point in self.graph:
            return self.graph[point]
        return set()

    def __str__(self) -> str:
        res = ""
        for point in self.graph:
            res += "\n" + str(point) + ": "
            for edge in self.graph[point]:
                res += str(edge)
        return res

    def __repr__(self) -> str:
        return self.__str__()

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, Graph):
            return False

        return self.graph == other.graph and self.edges == other.edges and self.polygons == other.polygons
