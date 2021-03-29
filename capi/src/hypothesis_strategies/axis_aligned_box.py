import typing

import hypothesis.strategies as st
from hypothesis import assume

from capi.src.datastructures.axis_aligned_box import AxisAlignedBox
from capi.src.dtos.coordinate import Coordinate
from capi.src.hypothesis_strategies.coordinate import coordinate_strategy


@st.composite
def axis_aligned_box_strategy(draw: typing.Callable):
    top_left = typing.cast(Coordinate, draw(coordinate_strategy()))
    bottom_right = typing.cast(Coordinate, draw(coordinate_strategy()))

    assume(bottom_right.longitude > top_left.longitude and bottom_right.latitude < top_left.latitude)

    return AxisAlignedBox(top_left, bottom_right)
