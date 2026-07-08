#include "pch.h"
#include "CLighting.h"

CLighting::CLighting() : nLightNumber(1), LightSource(nullptr) {
    Ambient = CRGB(0.35, 0.35, 0.40);
    LightSource = new CLightSource[nLightNumber];
    LightSource[0].SetPosition(2, 3, 4);
    LightSource[0].SetDiffuse(1.0, 1.0, 0.95);
    LightSource[0].SetSpecular(1.0, 1.0, 1.0);
}

CLighting::~CLighting() {
    delete[] LightSource;
}

CRGB CLighting::Illuminate(const CP3& ViewPoint, const CP3& Point, const CVector3& Normal, const CMaterial* pMaterial) {
    CRGB totalColor = Ambient * pMaterial->M_Ambient;

    CVector3 N = Normal.Normalize();

    for (int i = 0; i < nLightNumber; i++) {
        if (!LightSource[i].L_OnOff) continue;

        // Light direction
        CVector3 L(LightSource[i].L_Position.x - Point.x,
                    LightSource[i].L_Position.y - Point.y,
                    LightSource[i].L_Position.z - Point.z);
        double distance = L.Magnitude();
        L = L.Normalize();

        // Diffuse (Lambertian)
        double NdotL = N.Dot(L);
        if (NdotL < 0) NdotL = 0;
        CRGB diffuse = pMaterial->M_Diffuse * LightSource[i].L_Diffuse * NdotL;

        // Specular (Blinn-Phong: half-angle vector)
        CVector3 V(ViewPoint.x - Point.x, ViewPoint.y - Point.y, ViewPoint.z - Point.z);
        V = V.Normalize();
        CVector3 H = (L + V).Normalize();
        double NdotH = N.Dot(H);
        if (NdotH < 0) NdotH = 0;
        double specFactor = pow(NdotH, pMaterial->M_Shininess);
        CRGB specular = pMaterial->M_Specular * LightSource[i].L_Specular * specFactor;

        // Attenuation
        double atten = 1.0 / (LightSource[i].L_C0 + LightSource[i].L_C1 * distance + LightSource[i].L_C2 * distance * distance);

        totalColor = totalColor + (diffuse + specular) * atten;
    }

    totalColor.Normalize();
    return totalColor;
}
