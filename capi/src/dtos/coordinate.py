from pydantic import BaseModel  # pylint: disable=no-name-in-module


class Coordinate(BaseModel):
    latitude: float
    longitude: float
