#pragma once
#include "CLightSource.h"
#include "CMaterial.h"
#include "CVector3.h"
#include "CP3.h"

class CLighting {
public:
    int nLightNumber;
    CLightSource* LightSource;
    CRGB Ambient;

    CLighting();
    ~CLighting();

    // Blinn-Phong illumination at a point in world space
    CRGB Illuminate(const CP3& ViewPoint, const CP3& Point, const CVector3& Normal, const CMaterial* pMaterial);
};
