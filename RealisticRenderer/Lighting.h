#pragma once
#include "LightSource.h"
#include "Material.h"
#include "Vertex.h"

class CLighting {
public:
    CLighting();

    // Compute color at a vertex/world point using Phong model
    void ComputeVertexColor(
        const Vector4& worldPos, const Vector4& worldNormal,
        const Vector4& viewPos,
        const CLightSource& light, const CMaterial& mat,
        float& r, float& g, float& b) const;

    // Clamp color components to [0, 1]
    static void Clamp(float& r, float& g, float& b);
};
