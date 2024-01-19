#pragma once

#include "vector3.h"

struct Material {
    //coefficient de réflexion de la lumière ambiante
    double Ka;
    //coefficient de réflexion diffuse
    double Kd;

    inline Material(double _Ka): 
        Ka(_Ka) {}
    
    inline Material(double _Ka, double _Kd):
        Ka(_Ka), Kd(_Kd) {}

    inline ~Material() {}
};