//
// Created by James.Balajan on 31/03/2021.
//

#include "line_segment.hpp"

Coordinate LineSegment::get_endpoint_1() const { return _endpoint_1; }

Coordinate LineSegment::get_endpoint_2() const { return _endpoint_2; }

std::optional<Coordinate> LineSegment::intersection_with_ray(const Coordinate& ray_start, const Coordinate& ray_vector) const {
    /*
     * We use Cramer's rule to solve for scalars lambda_1 and lambda_2,
     * given the parametric representation of the line segment
     * x = lambda_1(a - b) + b where lambda_1 goes from [0, 1] (a, b represent the segment endpoints)
     * and parametric representation of the ray
     * x = lambda_2*v + c (v is the ray direction, c is its starting location) where lambda_2 goes from [0, infinity)
    */

    const auto d = _endpoint_1 - _endpoint_2;
    if (d == ray_vector || d == -ray_vector) {
        // Parallel check
        return {};
    }

    const auto e = ray_start - _endpoint_2;
    const auto determinant = (d.get_longitude() * ray_vector.get_latitude()) - (d.get_latitude() * ray_vector.get_longitude());
    const auto lambda_1 = ((e.get_longitude() * ray_vector.get_latitude()) - (e.get_latitude() * ray_vector.get_longitude())) / determinant;
    const auto lambda_2 = (-(d.get_longitude() * e.get_latitude()) + (d.get_latitude() * e.get_longitude())) / determinant;

    if (lambda_1 < 0 || lambda_1 > 1 || lambda_2 < 0) {
        return {};
    }

    return ray_start + (ray_vector * lambda_2);
}

bool LineSegment::operator==(const LineSegment &other) const {
    return ((_endpoint_1 == other._endpoint_1) && (_endpoint_2 == other._endpoint_2)) ||
            ((_endpoint_1 == other._endpoint_2) && (_endpoint_2 == other._endpoint_1));
}

bool LineSegment::operator!=(const LineSegment &other) const {
    return !((*this) == other);
}
