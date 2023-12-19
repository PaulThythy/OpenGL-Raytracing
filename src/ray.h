#pragma once

#include "vector3.h"

struct Ray {
    Vector3 origin;
    Vector3 direction;

    inline Ray(const Vector3& _origin, const Vector3& _direction): origin(_origin), direction(_direction) {}

    inline ~Ray() {}

    inline Vector3& getPoint(double t) {
        Vector3 point;
        point.x = this->origin.x + this->direction.x * t;
        point.y = this->origin.y + this->direction.y * t;
        point.z = this->origin.z + this->direction.z * t;
        return point;
    }
};