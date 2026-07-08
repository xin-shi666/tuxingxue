#pragma once
#include "CP3.h"
#include "CRGB.h"

class CLightSource {
public:
    CRGB L_Diffuse;
    CRGB L_Specular;
    CP3 L_Position;
    double L_C0, L_C1, L_C2; // attenuation
    BOOL L_OnOff;

    CLightSource();
    void SetDiffuse(double r, double g, double b) { L_Diffuse = CRGB(r, g, b); }
    void SetSpecular(double r, double g, double b) { L_Specular = CRGB(r, g, b); }
    void SetPosition(double x, double y, double z) { L_Position = CP3(x, y, z); }
    void SetAttenuation(double c0, double c1, double c2) { L_C0 = c0; L_C1 = c1; L_C2 = c2; }
    void SetOnOff(BOOL on) { L_OnOff = on; }
};
