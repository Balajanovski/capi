from pydantic import BaseModel, validator  # pylint: disable=no-name-in-module


class Coordinate(BaseModel):
    latitude: float
    longitude: float

    # pylint: disable=no-self-argument, no-self-use
    @validator("latitude")
    def latitude_range(cls, v: float) -> float:
        return ((v + 90) % 180) - 90

    # pylint: disable=no-self-argument, no-self-use
    @validator("longitude")
    def longitude_range(cls, v: float) -> float:
        return ((v + 180) % 360) - 180
