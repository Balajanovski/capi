import typing

import hypothesis.strategies as st

from capi.src.implementation.dtos.coordinate import Coordinate

_ALLOW_INFINITY = False
_ALLOW_NAN = False
_MIN_LAT = -90
_MAX_LAT = 90
_MIN_LON = -180
_MAX_LON = 180


@st.composite
def coordinate_strategy(draw: typing.Callable):
    lat = draw(st.floats(allow_infinity=_ALLOW_INFINITY, allow_nan=_ALLOW_NAN, min_value=_MIN_LAT, max_value=_MAX_LAT))
    lon = draw(st.floats(allow_infinity=_ALLOW_INFINITY, allow_nan=_ALLOW_NAN, min_value=_MIN_LON, max_value=_MAX_LON))

    return Coordinate(
        latitude=lat,
        longitude=lon,
    )
