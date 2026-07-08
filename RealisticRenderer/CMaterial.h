#pragma once
#include "CRGB.h"

class CMaterial {
public:
    CRGB M_Ambient;
    CRGB M_Diffuse;
    CRGB M_Specular;
    CRGB M_Emission;
    double M_Shininess;

    CMaterial();
    void SetAmbient(double r, double g, double b) { M_Ambient = CRGB(r, g, b); }
    void SetDiffuse(double r, double g, double b) { M_Diffuse = CRGB(r, g, b); }
    void SetSpecular(double r, double g, double b) { M_Specular = CRGB(r, g, b); }
    void SetShininess(double n) { M_Shininess = n; }
};
