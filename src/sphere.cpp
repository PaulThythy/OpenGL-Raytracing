#include <iostream>
#include <cmath>

#include "sphere.h"

Vector3* Sphere::intersect(const Ray& ray) {
    Vector3 oc = ray.origin - center;
    double a = ray.direction.dot(ray.direction);
    double b = 2.0 * oc.dot(ray.direction);
    double c = oc.dot(oc) - radius * radius;
    double discriminant = b * b - 4 * a * c;

    if(discriminant < 0) {
        return nullptr;     //no intersection
    }
    else{
        float t = (-b - std::sqrt(discriminant)) / (2.0 * a);
        if(t > 0) {
            return new Vector3(ray.origin + ray.direction * t);
        }
        else {
            return nullptr;         //intersection behind ray origin
        }
    }
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