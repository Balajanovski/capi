//
// Created by James.Balajan on 7/04/2021.
//

#ifndef CAPI_CONSTANTS_HPP
#define CAPI_CONSTANTS_HPP

#endif // CAPI_CONSTANTS_HPP

static constexpr double EPSILON_TOLERANCE = 0.0000000000001;
static constexpr double EPSILON_TOLERANCE_SQUARED = EPSILON_TOLERANCE * EPSILON_TOLERANCE;
static constexpr double MAX_LONGITUDE = 180.0;
static constexpr double MIN_LONGITUDE = -180.0;
static constexpr double LONGITUDE_PERIOD = MAX_LONGITUDE - MIN_LONGITUDE + (EPSILON_TOLERANCE * 10);
static constexpr double MAX_PERIODIC_LONGITUDE = MAX_LONGITUDE + LONGITUDE_PERIOD;
static constexpr double MAX_LATITUDE = 90.0;
static constexpr double MIN_LATITUDE = -90.0;
