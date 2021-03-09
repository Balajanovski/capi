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
SOFTWARE.f
"""
import pickle
import typing
from multiprocessing import Pool

from tqdm import tqdm  # type: ignore

from capi.src.pyvisgraph.graph import Edge, Graph, Point
from capi.src.pyvisgraph.shortest_path import shortest_path
from capi.src.pyvisgraph.visible_vertices import closest_point, point_in_polygon, visible_vertices


class VisGraph:
    def __init__(self):
        self.graph: typing.Optional[Graph] = None
        self.visgraph: typing.Optional[Graph] = None

    def load(self, filename: str) -> None:
        """Load obstacle graph and visibility graph. """
        with open(filename, "rb") as load:
            self.graph, self.visgraph = pickle.load(load)

    def save(self, filename: str) -> None:
        """Save obstacle graph and visibility graph. """
        with open(filename, "wb") as output:
            pickle.dump((self.graph, self.visgraph), output, -1)

    def build(self, polygons: typing.Sequence[typing.Sequence[Point]], workers: int = 1, status: bool = True) -> None:
        """Build visibility graph based on a list of polygons.

        The polygons must be a list of polygons, where each polygon is a list of
        in-order (clockwise or counter clockwise) Points. It only one polygon,
        it must still be a list in a list, i.e. [[Point(0,0), Point(2,0),
        Point(2,1)]].
        Take advantage of processors with multiple cores by setting workers to
        the number of subprocesses you want. Defaults to 1, i.e. no subprocess
        will be started.
        Set status=False to turn off the statusbar when building.
        """

        self.graph = Graph(polygons)
        self.visgraph = Graph([])

        points = self.graph.get_points()
        batch_size = 10

        if workers == 1:
            for batch in tqdm(
                [points[i : i + batch_size] for i in range(0, len(points), batch_size)],
                disable=not status,
            ):
                for edge in _vis_graph(self.graph, batch):
                    self.visgraph.add_edge(edge)
        else:
            pool = Pool(workers)
            batches = [(self.graph, points[i : i + batch_size]) for i in range(0, len(points), batch_size)]

            results = list(
                tqdm(
                    pool.imap(_vis_graph_wrapper, batches),
                    total=len(batches),
                    disable=not status,
                )
            )
            for result in results:
                for edge in result:
                    self.visgraph.add_edge(edge)

    def find_visible(self, point: Point) -> typing.Sequence[Point]:
        """Find vertices visible from point."""
        self._guard_against_unloaded_graph(self.find_visible.__name__)

        return visible_vertices(point, typing.cast(Graph, self.graph))

    def update(
        self,
        points: typing.Sequence[Point],
        origin: typing.Optional[Point] = None,
        destination: typing.Optional[Point] = None,
    ) -> None:
        """Update visgraph by checking visibility of Points in list points."""
        self._guard_against_unloaded_graph(self.update.__name__)
        self._guard_against_unloaded_visgraph(self.update.__name__)

        for point in points:
            for v in visible_vertices(point, typing.cast(Graph, self.graph), origin=origin, destination=destination):
                typing.cast(Graph, self.visgraph).add_edge(Edge(point, v))

    def shortest_path(self, origin: Point, destination: Point) -> typing.Sequence[Point]:
        """Find and return shortest path between origin and destination.

        Will return in-order list of Points of the shortest path found. If
        origin or destination are not in the visibility graph, their respective
        visibility edges will be found, but only kept temporarily for finding
        the shortest path.
        """
        self._guard_against_unloaded_graph(self.shortest_path.__name__)
        self._guard_against_unloaded_visgraph(self.shortest_path.__name__)

        graph = typing.cast(Graph, self.graph)
        visgraph = typing.cast(Graph, self.visgraph)

        origin_exists = origin in visgraph
        dest_exists = destination in visgraph
        if origin_exists and dest_exists:
            return shortest_path(visgraph, origin, destination)
        orgn = None if origin_exists else origin
        dest = None if dest_exists else destination
        add_to_visg = Graph([])
        if not origin_exists:
            for v in visible_vertices(origin, graph, destination=dest):
                add_to_visg.add_edge(Edge(origin, v))
        if not dest_exists:
            for v in visible_vertices(destination, graph, origin=orgn):
                add_to_visg.add_edge(Edge(destination, v))
        return shortest_path(visgraph, origin, destination, add_to_visg)

    def point_in_polygon(self, point: Point) -> int:
        """Return polygon_id if point in a polygon, -1 otherwise."""
        self._guard_against_unloaded_graph(self.point_in_polygon.__name__)

        return point_in_polygon(point, typing.cast(Graph, self.graph))

    def closest_point(self, point: Point, polygon_id: int, length: float = 0.001) -> Point:
        """Return closest Point outside polygon from point.

        Note method assumes point is inside the polygon, no check is
        performed.
        """
        self._guard_against_unloaded_graph(self.closest_point.__name__)

        return closest_point(point, typing.cast(Graph, self.graph), polygon_id, length)

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, VisGraph):
            return False

        return self.visgraph == other.visgraph and self.graph == other.graph

    def _guard_against_unloaded_graph(self, operation_name: str) -> None:
        if self.graph is None:
            raise RuntimeError(f"Attempted to perform operation {operation_name} on unloaded graph")

    def _guard_against_unloaded_visgraph(self, operation_name: str) -> None:
        if self.visgraph is None:
            raise RuntimeError(f"Attempted to perform operation {operation_name} on unloaded visgraph")


def _vis_graph_wrapper(batch: typing.Tuple[Graph, typing.Sequence[Point]]) -> typing.Sequence[Edge]:
    graph, points = batch
    return _vis_graph(graph, points)


def _vis_graph(graph: Graph, points: typing.Sequence[Point]) -> typing.Sequence[Edge]:
    visible_edges = []
    for point in points:
        for vertex_visible_from_point in visible_vertices(point, graph, scan="half"):
            visible_edges.append(Edge(point, vertex_visible_from_point))
    return visible_edges
