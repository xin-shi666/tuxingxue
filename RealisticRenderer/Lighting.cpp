#include "stdafx.h"
#include "Lighting.h"
#include <algorithm>

CLighting::CLighting() {}

void CLighting::ComputeVertexColor(
    const Vector4& worldPos, const Vector4& worldNormal,
    const Vector4& viewPos,
    const CLightSource& light, const CMaterial& mat,
    float& r, float& g, float& b) const
{
    // Light direction
    Vector4 lightPos(light.GetPosX(), light.GetPosY(), light.GetPosZ());
    Vector4 L = (lightPos - worldPos).Normalize();

    // View direction
    Vector4 V = (viewPos - worldPos).Normalize();

    // Normal
    Vector4 N = worldNormal.Normalize();

    float NdotL = N.Dot(L);
    if (NdotL < 0) NdotL = 0;

    // Ambient
    r = mat.GetKaR() * light.GetAmbientR();
    g = mat.GetKaG() * light.GetAmbientG();
    b = mat.GetKaB() * light.GetAmbientB();

    // Diffuse (Lambertian)
    r += mat.GetKdR() * light.GetDiffuseR() * NdotL;
    g += mat.GetKdG() * light.GetDiffuseG() * NdotL;
    b += mat.GetKdB() * light.GetDiffuseB() * NdotL;

    // Specular (Phong)
    Vector4 R = (N * (2.0f * NdotL) - L).Normalize();
    float RdotV = R.Dot(V);
    if (RdotV < 0) RdotV = 0;
    float spec = powf(RdotV, mat.GetShininess());

    r += mat.GetKsR() * light.GetSpecularR() * spec;
    g += mat.GetKsG() * light.GetSpecularG() * spec;
    b += mat.GetKsB() * light.GetSpecularB() * spec;

    Clamp(r, g, b);
}

void CLighting::Clamp(float& r, float& g, float& b) {
    r = std::min(1.0f, std::max(0.0f, r));
    g = std::min(1.0f, std::max(0.0f, g));
    b = std::min(1.0f, std::max(0.0f, b));
}
