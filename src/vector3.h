#pragma once

#include <cmath>

struct Vector3 {
    double x, y, z;

    inline Vector3() {}
    inline Vector3(double _x, double _y, double _z):
        x(_x), y(_y), z(_z) {}

    inline ~Vector3() {}
    
    inline double magnitude() {
        return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    }

    inline double magnitudeSquared() {
        return pow(magnitude(), 2);
    }

    inline Vector3 normalize() {
        double mag = magnitude();
        Vector3 v(x/mag, y/mag, z/mag);
        return v;
    }

    inline float dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    inline Vector3 cross(const Vector3& other) const {
        Vector3 v(y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
        return v;
    }

    inline void operator+=(const Vector3& v) {
        this->x += v.x; this->y += v.y; this->z += v.z;
    }
};

inline Vector3 operator-(const Vector3& v1, const Vector3& v2) {
    return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline Vector3 operator+(const Vector3& v1, const Vector3& v2) {
    return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline Vector3 operator*(const Vector3& v, const double scalar) {
    return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
}