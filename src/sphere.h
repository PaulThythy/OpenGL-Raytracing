#ifndef PARTICLESYSTEM_SPHERE_H
#define PARTICLESYSTEM_SPHERE_H

#include <iostream>
#include <vector>

#include "vector3.h"
#include "ray.h"
#include "material.h"

class Sphere {
public:
    Vector3 center;
    double radius;
    Material material;

    Sphere(const Vector3& _center, const double _radius, const Material& _material): 
        center(_center), radius(_radius), material(_material) {}

    ~Sphere() {}

    Vector3* intersect(const Ray& ray);

    double distance(const Vector3& point);
    bool isInside(const Vector3& point);
    bool isOn(const Vector3& point);
    bool isOutside(const Vector3& point);
    Vector3 normalAt(const Vector3* intersection) const;
};

#endif