#include "pch.h"
#include "CMaterial.h"

CMaterial::CMaterial() {
    M_Ambient = CRGB(0.3, 0.3, 0.3);
    M_Diffuse = CRGB(0.9, 0.9, 0.85);
    M_Specular = CRGB(0.6, 0.6, 0.6);
    M_Emission = CRGB(0, 0, 0);
    M_Shininess = 32.0;
}
