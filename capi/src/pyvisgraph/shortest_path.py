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

from capi.src.pyvisgraph.graph import Graph, Point
from capi.src.pyvisgraph.priority_dict import PriorityDict
from capi.src.pyvisgraph.visible_vertices import euclidean_distance


def shortest_path(
    graph: Graph, origin: Point, destination: Point, add_to_visgraph: typing.Optional[Graph] = None
) -> typing.Sequence[Point]:
    came_from = astar(graph, origin, destination, add_to_visgraph)
    path = []
    while 1:
        path.append(destination)
        if destination == origin:
            break

        if came_from[destination] is None:
            raise RuntimeError(f"came_from for destinationn {destination} unexpectedly None")
        destination = typing.cast(Point, came_from[destination])
    path.reverse()
    return path


def astar(
    graph: Graph, origin: Point, destination: Point, add_to_visgraph: typing.Optional[Graph] = None
) -> typing.Dict[Point, typing.Optional[Point]]:
    """
    point_a* search algorithm, using Euclidean distance heuristic
    Note that this is a modified version of an
    point_a* implementation by Amit Patel.
    https://www.redblobgames.com/pathfinding/a-star/implementation.html
    """
    frontier = PriorityDict[Point]()
    frontier[origin] = 0
    came_from: typing.Dict[Point, typing.Optional[Point]] = {origin: None}
    cost_so_far: typing.Dict[Point, float] = {origin: 0}

    while len(frontier) > 0:
        current = frontier.pop_smallest()
        if current == destination:
            break

        edges = graph[current]
        if add_to_visgraph is not None and len(add_to_visgraph[current]) > 0:
            edges = add_to_visgraph[current] | graph[current]
        for edge in edges:
            adjacent_point = edge.get_adjacent(current)
            new_cost = cost_so_far[current] + euclidean_distance(current, adjacent_point)
            if adjacent_point not in cost_so_far or new_cost < cost_so_far[adjacent_point]:
                cost_so_far[adjacent_point] = new_cost
                priority = new_cost + euclidean_distance(adjacent_point, destination)
                frontier[adjacent_point] = priority
                came_from[adjacent_point] = current

    return came_from
