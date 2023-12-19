#include <iostream>
#include <cmath>

#include "sphere.h"

std::vector<Vector3&> Sphere::AppendAllIntersections(const Ray& ray) {
    std::vector<Vector3> intersections;

    // Calculate the coefficients of the quadratic equation
    //     au^2 + bu + c = 0.
    // Solving this equation gives us the value of u
    // for any intersection points.

    const Vector3 displacement = ray.origin - this->center;
    const double a = ray.direction.magnitudeSquared();
    //const double b = 
}

double Sphere::distance(const Vector3& point) {
    int x1 = pow(point.x - this->center.x, 2);
    int y1 = pow(point.y - this->center.y, 2);
    int z1 = pow(point.z - this->center.z, 2);

    return (x1 + y1 + z1);
}

bool Sphere::isInside(const Vector3& point) {
    return distance(point) < (this->radius * this->radius);
}

bool Sphere::isOn(const Vector3& point) {
    return distance(point) == (this->radius * this->radius);
}

bool Sphere::isOutside(const Vector3& point) {
    return distance(point) > (this->radius * this->radius);
}