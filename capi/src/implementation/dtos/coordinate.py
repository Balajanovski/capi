from dataclasses import dataclass


@dataclass
class Coordinate:
    latitude: float
    longitude: float

    class Config:
        frozen = True


def construct_coord_in_lat_lon_range(latitude: float, longitude: float) -> Coordinate:
    return Coordinate(
        latitude=((latitude + 90) % 180) - 90,
        longitude=((longitude + 180) % 360) - 180,
    )
