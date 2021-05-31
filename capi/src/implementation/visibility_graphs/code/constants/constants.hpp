//
// Created by James.Balajan on 7/04/2021.
//

#ifndef CAPI_CONSTANTS_HPP
#define CAPI_CONSTANTS_HPP

// WHERE YOU LEFT OFF
// EPSILONS ARE CAUSING TROUBLE!!!!!

static constexpr float EPSILON_TOLERANCE = 0.001f;
static constexpr float EPSILON_TOLERANCE_SQUARED = EPSILON_TOLERANCE * EPSILON_TOLERANCE;
static constexpr float MAX_LONGITUDE = 180.0f;
static constexpr float MIN_LONGITUDE = -180.0f;
static constexpr float LONGITUDE_PERIOD = MAX_LONGITUDE - MIN_LONGITUDE + EPSILON_TOLERANCE;
static constexpr float MAX_PERIODIC_LONGITUDE = MAX_LONGITUDE + LONGITUDE_PERIOD;
static constexpr float MAX_LATITUDE = 90.0f;
static constexpr float MIN_LATITUDE = -90.0f;

#endif // CAPI_CONSTANTS_HPP
