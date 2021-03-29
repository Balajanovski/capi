import typing

import hypothesis.strategies as st

from capi.src.datastructures.polygon import Polygon
from capi.src.hypothesis_strategies.coordinate import coordinate_strategy


@st.composite
def polygon_strategy(draw: typing.Callable, min_num_vertices: int = 3, max_num_vertices: int = 10):
    vertices = draw(st.lists(coordinate_strategy(), min_size=min_num_vertices, max_size=max_num_vertices))
    return Polygon(vertices)
