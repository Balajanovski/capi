from pydantic import BaseModel  # pylint: disable=no-name-in-module


class Coordinate(BaseModel):
    latitude: float
    longitude: float

    class Config:
        frozen = True


def construct_coord_in_lat_lon_range(latitude: float, longitude: float) -> Coordinate:
    return Coordinate.construct(
        latitude=((latitude + 90) % 180) - 90,
        longitude=((longitude + 180) % 360) - 180,
    )
