#pragma once

#include "vector3.h"

enum LightType {
    AMBIENT,
    DIRECTIONAL,
    POINT
};

struct Light {
    LightType type;
    Vector3 position;       //utilisé pour les lumières directionnelles et ponctuelles
    Vector3 intensity;      // Intensité pour tout les types de lumière

    //constructeur pour la lumière ambiante
    inline Light(LightType t, const Vector3& _intensity):
        type(t), intensity(_intensity) {}
    
    //constructeur pour les lumières directionnelles et ponctuelles
    inline Light(LightType t, const Vector3& _position, const Vector3& _intensity): 
        type(t), position(_position), intensity(_intensity) {}
    
    inline ~Light() {}
};