import typing

import hypothesis.strategies as st
from hypothesis import assume

from capi.src.hypothesis_strategies.coordinate import coordinate_strategy
from capi.src.implementation.datastructures.axis_aligned_box import AxisAlignedBox
from capi.src.implementation.dtos.coordinate import Coordinate


@st.composite
def axis_aligned_box_strategy(draw: typing.Callable):
    top_left = typing.cast(Coordinate, draw(coordinate_strategy()))
    bottom_right = typing.cast(Coordinate, draw(coordinate_strategy()))

    assume(bottom_right.longitude > top_left.longitude and bottom_right.latitude < top_left.latitude)

    return AxisAlignedBox(top_left, bottom_right)
