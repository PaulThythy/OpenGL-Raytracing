#ifndef PARTICLESYSTEM_SPHERE_H
#define PARTICLESYSTEM_SPHERE_H

#include <iostream>
#include <vector>

#include "vector3.h"
#include "ray.h"

class Sphere {
private:
    Vector3 center;
    double radius;

public:
    Sphere(const Vector3& _center, const double _radius): center(_center), radius(_radius) {}

    ~Sphere() {}

    Vector3* intersect(const Ray& ray);

    double distance(const Vector3& point);
    bool isInside(const Vector3& point);
    bool isOn(const Vector3& point);
    bool isOutside(const Vector3& point);
};

#endif