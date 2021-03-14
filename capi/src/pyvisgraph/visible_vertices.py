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

import typing
from math import pi

from capi.src.pyvisgraph.geometry import (
    CCW,
    COLLINEAR,
    CW,
    INF,
    angle_arctan,
    angle_cosine_theorem,
    counter_clockwise,
    edge_in_polygon,
    edge_intersect,
    euclidean_distance,
    on_segment,
    point_edge_distance,
)
from capi.src.pyvisgraph.graph import Edge, Graph, Point


def visible_vertices(
    point: Point,
    graph: Graph,
    origin: typing.Optional[Point] = None,
    destination: typing.Optional[Point] = None,
    scan: str = "full",
) -> typing.Sequence[Point]:
    """Returns list of Points in graph visible by point.

    If origin and/or destination Points are given, these will also be checked
    for visibility. scan 'full' will check for visibility against all points in
    graph, 'half' will check for visibility against half the points. This saves
    running time when building a complete visibility graph, as the points
    that are not checked will eventually be 'point'.
    """
    edges = graph.get_edges()
    graph_points = graph.get_points()

    if origin:
        graph_points.append(origin)
    if destination:
        graph_points.append(destination)
    graph_points.sort(key=lambda p: (angle_arctan(point, p), euclidean_distance(point, p)))

    # Initialize open_edges with any intersecting edges on the half line from
    # point along the positive x-axis
    open_edges = OpenEdges()
    point_inf = Point(INF, point.y)
    for edge in edges:
        if point in edge:
            continue
        if edge_intersect(point, point_inf, edge):
            if on_segment(point, edge.endpoint_1, point_inf):
                continue
            if on_segment(point, edge.endpoint_2, point_inf):
                continue
            open_edges.insert(point, point_inf, edge)

    visible = []
    prev = None
    prev_visible = None
    for graph_point in graph_points:
        if graph_point == point:
            continue
        if scan == "half" and angle_arctan(point, graph_point) > pi:
            break

        # Update open_edges - remove clock wise edges incident on point
        if open_edges:
            for edge in graph[graph_point]:
                if counter_clockwise(point, graph_point, edge.get_adjacent(graph_point)) == CW:
                    open_edges.delete(point, graph_point, edge)

        # Check if point is visible from point
        is_visible = False
        # ...Non-collinear points
        if (
            prev is None
            or counter_clockwise(point, prev, graph_point) != COLLINEAR
            or not on_segment(point, prev, graph_point)
        ):
            if len(open_edges) == 0:
                is_visible = True
            elif not edge_intersect(point, graph_point, open_edges.smallest()):
                is_visible = True
        # ...For collinear points, if previous point was not visible, point is not
        elif not prev_visible:
            is_visible = False
        # ...For collinear points, if previous point was visible, need to check
        # that the edge from prev to point does not intersect any open edge.
        else:
            is_visible = True
            for edge in open_edges:
                if prev not in edge and edge_intersect(prev, graph_point, edge):
                    is_visible = False
                    break
            if is_visible and edge_in_polygon(prev, graph_point, graph):
                is_visible = False

        # Check if the visible edge is interior to its polygon
        if is_visible and graph_point not in graph.get_adjacent_points(point):
            is_visible = not edge_in_polygon(point, graph_point, graph)

        if is_visible:
            visible.append(graph_point)

        # Update open_edges - Add counter clock wise edges incident on point
        for edge in graph[graph_point]:
            if (point not in edge) and counter_clockwise(point, graph_point, edge.get_adjacent(graph_point)) == CCW:
                open_edges.insert(point, graph_point, edge)

        prev = graph_point
        prev_visible = is_visible
    return visible


class OpenEdges:
    def __init__(self):
        self._open_edges: typing.List[Edge] = []

    def insert(self, point_1: Point, point_2: Point, edge: Edge) -> None:
        self._open_edges.insert(self._index(point_1, point_2, edge), edge)

    def delete(self, point_1: Point, point_2: Point, edge: Edge) -> None:
        index = self._index(point_1, point_2, edge) - 1
        if self._open_edges[index] == edge:
            del self._open_edges[index]

    def smallest(self) -> Edge:
        return self._open_edges[0]

    def _index(self, point_1: Point, point_2: Point, edge: Edge) -> int:
        low = 0
        high = len(self._open_edges)
        while low < high:
            mid = (low + high) // 2
            if self._less_than(point_1, point_2, edge, self._open_edges[mid]):
                high = mid
            else:
                low = mid + 1
        return low

    def __len__(self) -> int:
        return len(self._open_edges)

    def __getitem__(self, index: int) -> Edge:
        return self._open_edges[index]

    @staticmethod
    def _less_than(point_1: Point, point_2: Point, edge1: Edge, edge2: Edge) -> bool:
        """Return True if edge1 is smaller than edge2, False otherwise."""
        if edge1 == edge2:
            return False
        if not edge_intersect(point_1, point_2, edge2):
            return True
        edge1_dist = point_edge_distance(point_1, point_2, edge1)
        edge2_dist = point_edge_distance(point_1, point_2, edge2)
        if edge1_dist > edge2_dist:
            return False
        if edge1_dist < edge2_dist:
            return True
        # If the distance is equal, we need to compare on the edge angles.
        if edge1_dist == edge2_dist:
            if edge1.endpoint_1 in edge2:
                same_point = edge1.endpoint_1
            else:
                same_point = edge1.endpoint_2
            angle_edge1 = angle_cosine_theorem(point_1, point_2, edge1.get_adjacent(same_point))
            angle_edge2 = angle_cosine_theorem(point_1, point_2, edge2.get_adjacent(same_point))
            if angle_edge1 < angle_edge2:
                return True
        return False
