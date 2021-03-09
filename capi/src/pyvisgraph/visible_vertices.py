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
from math import acos, atan, pi, sqrt

from capi.src.pyvisgraph.graph import Edge, Graph, Point

INF = 10000
CCW = 1
CW = -1
COLLINEAR = 0
"""Due to floating point representation error, some functions need to
   truncate floating point numbers to a certain tolerance."""
COLIN_TOLERANCE = 10
T = 10 ** COLIN_TOLERANCE
T2 = 10.0 ** COLIN_TOLERANCE


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
    graph_points.sort(key=lambda p: (angle(point, p), euclidean_distance(point, p)))

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
        if scan == "half" and angle(point, graph_point) > pi:
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


def polygon_crossing(point_1: Point, poly_edges: typing.Set[Edge]) -> bool:
    """Returns True if Point endpoint_1 is internal to the polygon. The polygon is
    defined by the Edges in poly_edges. Uses crossings algorithm and takes into
    account edges that are collinear to endpoint_1."""
    point_2 = Point(INF, point_1.y)
    intersect_count = 0
    for edge in poly_edges:
        if point_1.y < edge.endpoint_1.y and point_1.y < edge.endpoint_2.y:
            continue
        if point_1.y > edge.endpoint_1.y and point_1.y > edge.endpoint_2.y:
            continue
        if point_1.x > edge.endpoint_1.x and point_1.x > edge.endpoint_2.x:
            continue
        # Deal with points collinear to endpoint_1
        edge_p1_collinear = counter_clockwise(point_1, edge.endpoint_1, point_2) == COLLINEAR
        edge_p2_collinear = counter_clockwise(point_1, edge.endpoint_2, point_2) == COLLINEAR
        if edge_p1_collinear and edge_p2_collinear:
            continue
        if edge_p1_collinear or edge_p2_collinear:
            collinear_point = edge.endpoint_1 if edge_p1_collinear else edge.endpoint_2
            if edge.get_adjacent(collinear_point).y > point_1.y:
                intersect_count += 1
        elif edge_intersect(point_1, point_2, edge):
            intersect_count += 1
    if intersect_count % 2 == 0:
        return False
    return True


def edge_in_polygon(point_1: Point, point_2: Point, graph: Graph) -> bool:
    """Return true if the edge from endpoint_1 to endpoint_2 is interior to any polygon
    in graph."""
    if point_1.polygon_id != point_2.polygon_id:
        return False
    if point_1.polygon_id == -1 or point_2.polygon_id == -1:
        return False
    mid_point = Point((point_1.x + point_2.x) / 2, (point_1.y + point_2.y) / 2)
    return polygon_crossing(mid_point, graph.polygons[point_1.polygon_id])


def point_in_polygon(point: Point, graph: Graph) -> int:
    """Return true if the point point is interior to any polygon in graph."""
    for polygon in graph.polygons:
        if polygon_crossing(point, graph.polygons[polygon]):
            return polygon
    return -1


def unit_vector(vector_base: Point, vector_head: Point) -> Point:
    magnitude = euclidean_distance(vector_base, vector_head)
    return Point((vector_head.x - vector_base.x) / magnitude, (vector_head.y - vector_base.y) / magnitude)


def closest_point(point: Point, graph: Graph, polygon_id: int, length: float = 0.001) -> Point:
    """Assumes point is interior to the polygon with polygon_id. Returns the
    closest point vector_base outside the polygon to point, where the distance from vector_base to
    the intersect point from point to the edge of the polygon is length."""
    polygon_edges = graph.polygons[polygon_id]
    close_point: typing.Optional[Point] = None
    close_edge: typing.Optional[Edge] = None
    close_dist: typing.Optional[float] = None
    # Finds point closest to point, but on a edge of the polygon.
    # Solution from http://stackoverflow.com/a/6177788/4896361
    for edge in polygon_edges:
        num = (point.x - edge.endpoint_1.x) * (edge.endpoint_2.x - edge.endpoint_1.x) + (
            point.y - edge.endpoint_1.y
        ) * (edge.endpoint_2.y - edge.endpoint_1.y)
        denom = (edge.endpoint_2.x - edge.endpoint_1.x) ** 2 + (edge.endpoint_2.y - edge.endpoint_1.y) ** 2
        percent_dist_between_edge_endpoints_for_closest_pt = num / denom
        uncorrected_closest_pt_for_current_edge = Point(
            edge.endpoint_1.x
            + percent_dist_between_edge_endpoints_for_closest_pt * (edge.endpoint_2.x - edge.endpoint_1.x),
            edge.endpoint_1.y
            + percent_dist_between_edge_endpoints_for_closest_pt * (edge.endpoint_2.y - edge.endpoint_1.y),
        )
        closest_pt_for_current_edge = uncorrected_closest_pt_for_current_edge
        if percent_dist_between_edge_endpoints_for_closest_pt < 0:
            closest_pt_for_current_edge = edge.endpoint_1
        elif percent_dist_between_edge_endpoints_for_closest_pt > 1:
            closest_pt_for_current_edge = edge.endpoint_2
        distance = euclidean_distance(point, closest_pt_for_current_edge)
        if close_dist is None or distance < close_dist:
            close_dist = distance
            close_point = closest_pt_for_current_edge
            close_edge = edge

    if close_edge is None or close_point is None or close_dist is None:
        raise RuntimeError(
            f"Close edge, or point, or dist is unexpectedly None. "
            f"(Close Edge: {close_edge}, Close Point: {close_point}, Close Dist: {close_dist})"
        )

    # Extend the newly found point so it is outside the polygon by `length`.
    if close_edge is not None and close_point in close_edge:
        close = close_edge.endpoint_1 if close_point == close_edge.endpoint_1 else close_edge.endpoint_2
        edges = list(graph[close])
        vector_1 = unit_vector(close, edges[0].get_adjacent(close))
        vector_2 = unit_vector(close, edges[1].get_adjacent(close))
        vsum = unit_vector(Point(0, 0), Point(vector_1.x + vector_2.x, vector_1.y + vector_2.y))
        close1 = Point(close.x + (vsum.x * length), close.y + (vsum.y * length))
        close2 = Point(close.x - (vsum.x * length), close.y - (vsum.y * length))
        if point_in_polygon(close1, graph) == -1:
            return close1
        return close2

    v = unit_vector(point, close_point)
    return Point(close_point.x + v.x * length, close_point.y + v.y * length)


def euclidean_distance(point_1: Point, point_2: Point) -> float:
    """Return the Euclidean distance between two Points."""
    return sqrt((point_2.x - point_1.x) ** 2 + (point_2.y - point_1.y) ** 2)


def intersect_point(point_1: Point, point_2: Point, edge: Edge) -> typing.Optional[Point]:
    """Return intersect Point where the edge from endpoint_1, endpoint_2 intersects edge"""
    if point_1 in edge:
        return point_1
    if point_2 in edge:
        return point_2
    if edge.endpoint_1.x == edge.endpoint_2.x:
        if point_1.x == point_2.x:
            return None
        pslope = (point_1.y - point_2.y) / (point_1.x - point_2.x)
        intersect_x = edge.endpoint_1.x
        intersect_y = pslope * (intersect_x - point_1.x) + point_1.y
        return Point(intersect_x, intersect_y)

    if point_1.x == point_2.x:
        eslope = (edge.endpoint_1.y - edge.endpoint_2.y) / (edge.endpoint_1.x - edge.endpoint_2.x)
        intersect_x = point_1.x
        intersect_y = eslope * (intersect_x - edge.endpoint_1.x) + edge.endpoint_1.y
        return Point(intersect_x, intersect_y)

    pslope = (point_1.y - point_2.y) / (point_1.x - point_2.x)
    eslope = (edge.endpoint_1.y - edge.endpoint_2.y) / (edge.endpoint_1.x - edge.endpoint_2.x)
    if eslope == pslope:
        return None
    intersect_x = (eslope * edge.endpoint_1.x - pslope * point_1.x + point_1.y - edge.endpoint_1.y) / (eslope - pslope)
    intersect_y = eslope * (intersect_x - edge.endpoint_1.x) + edge.endpoint_1.y
    return Point(intersect_x, intersect_y)


def point_edge_distance(point_1: Point, point_2: Point, edge: Edge) -> float:
    """Return the Eucledian distance from endpoint_1 to intersect point with edge.
    Assumes the line going from endpoint_1 to endpoint_2 intersects edge before reaching endpoint_2."""
    intersection_point = intersect_point(point_1, point_2, edge)
    if intersection_point is not None:
        return euclidean_distance(point_1, intersection_point)
    return 0


def angle(center: Point, point: Point) -> float:
    """Return the angle (radian) of point from center of the radian circle.
     ------point
     |   /
     |  /
    vector_base|a/
    """
    delta_x = point.x - center.x
    delta_y = point.y - center.y
    if delta_x == 0:
        if delta_y < 0:
            return pi * 3 / 2
        return pi / 2
    if delta_y == 0:
        if delta_x < 0:
            return pi
        return 0
    if delta_x < 0:
        return pi + atan(delta_y / delta_x)
    if delta_y < 0:
        return 2 * pi + atan(delta_y / delta_x)
    return atan(delta_y / delta_x)


def angle2(point_a: Point, point_b: Point, point_c: Point) -> float:
    """Return angle point_b (radian) between point_b and point_c.
           vector_base
         /  \
       /    point_b\
      a-------b
    """
    side_length_a_squared = (point_c.x - point_b.x) ** 2 + (point_c.y - point_b.y) ** 2
    side_length_b_squared = (point_c.x - point_a.x) ** 2 + (point_c.y - point_a.y) ** 2
    side_length_c_squared = (point_b.x - point_a.x) ** 2 + (point_b.y - point_a.y) ** 2
    cos_value = (side_length_a_squared + side_length_c_squared - side_length_b_squared) / (
        2 * sqrt(side_length_a_squared) * sqrt(side_length_c_squared)
    )
    return acos(int(cos_value * T) / T2)


def counter_clockwise(point_a: Point, point_b: Point, point_c: Point) -> int:
    """Return 1 if counter clockwise, -1 if clock wise, 0 if collinear """
    #  Rounding this way is faster than calling round()
    area = (
        int(((point_b.x - point_a.x) * (point_c.y - point_a.y) - (point_b.y - point_a.y) * (point_c.x - point_a.x)) * T)
        / T2
    )
    if area > 0:
        return 1
    if area < 0:
        return -1
    return 0


def on_segment(point_p: Point, point_q: Point, point_r: Point) -> bool:
    """Given three collinear points point, point_q, point_r, the function checks if point point_q
    lies on line segment 'pr'."""
    if (point_q.x <= max(point_p.x, point_r.x)) and (point_q.x >= min(point_p.x, point_r.x)):
        if (point_q.y <= max(point_p.y, point_r.y)) and (point_q.y >= min(point_p.y, point_r.y)):
            return True
    return False


def edge_intersect(edge_1_endpoint_1: Point, edge_1_endpoint_2: Point, edge: Edge) -> bool:
    """Return True if edge from point_a, point_b interects edge.
    http://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/"""
    edge_2_endpoint_1 = edge.endpoint_1
    edge_2_endpoint_2 = edge.endpoint_2
    orientation_1 = counter_clockwise(edge_1_endpoint_1, edge_1_endpoint_2, edge_2_endpoint_1)
    orientation_2 = counter_clockwise(edge_1_endpoint_1, edge_1_endpoint_2, edge_2_endpoint_2)
    orientation_3 = counter_clockwise(edge_2_endpoint_1, edge_2_endpoint_2, edge_1_endpoint_1)
    orientation_4 = counter_clockwise(edge_2_endpoint_1, edge_2_endpoint_2, edge_1_endpoint_2)

    # General case
    if orientation_1 != orientation_2 and orientation_3 != orientation_4:
        return True
    # endpoint_1, edge_1_endpoint_2 and endpoint_2 are colinear and endpoint_2 lies on segment p1q1
    if orientation_1 == COLLINEAR and on_segment(edge_1_endpoint_1, edge_2_endpoint_1, edge_1_endpoint_2):
        return True
    # endpoint_1, edge_1_endpoint_2 and endpoint_2 are colinear and q2 lies on segment p1q1
    if orientation_2 == COLLINEAR and on_segment(edge_1_endpoint_1, edge_2_endpoint_2, edge_1_endpoint_2):
        return True
    # endpoint_2, q2 and endpoint_1 are colinear and endpoint_1 lies on segment p2q2
    if orientation_3 == COLLINEAR and on_segment(edge_2_endpoint_1, edge_1_endpoint_1, edge_2_endpoint_2):
        return True
    # endpoint_2, q2 and edge_1_endpoint_2 are colinear and edge_1_endpoint_2 lies on segment p2q2
    if orientation_4 == COLLINEAR and on_segment(edge_2_endpoint_1, edge_1_endpoint_2, edge_2_endpoint_2):
        return True
    return False


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
            angle_edge1 = angle2(point_1, point_2, edge1.get_adjacent(same_point))
            angle_edge2 = angle2(point_1, point_2, edge2.get_adjacent(same_point))
            if angle_edge1 < angle_edge2:
                return True
        return False
