#include "pch.h"
#include "CMaterial.h"

CMaterial::CMaterial() {
    M_Ambient = CRGB(0.2, 0.2, 0.2);
    M_Diffuse = CRGB(0.8, 0.8, 0.8);
    M_Specular = CRGB(1.0, 1.0, 1.0);
    M_Emission = CRGB(0, 0, 0);
    M_Shininess = 10.0;
}
