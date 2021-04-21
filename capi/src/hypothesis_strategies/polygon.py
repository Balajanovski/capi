import typing

import hypothesis.strategies as st

from capi.src.hypothesis_strategies.coordinate import coordinate_strategy
from capi.src.implementation.datastructures.polygon import Polygon


@st.composite
def polygon_strategy(draw: typing.Callable, min_num_vertices: int = 3, max_num_vertices: int = 10):
    vertices = draw(st.lists(coordinate_strategy(), min_size=min_num_vertices, max_size=max_num_vertices, unique=True))
    return Polygon(vertices)
