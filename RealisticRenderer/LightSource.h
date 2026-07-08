#pragma once

class CLightSource {
public:
    CLightSource();

    void SetPosition(float x, float y, float z) { m_posX = x; m_posY = y; m_posZ = z; }
    void SetAmbient(float r, float g, float b) { m_ambR = r; m_ambG = g; m_ambB = b; }
    void SetDiffuse(float r, float g, float b) { m_difR = r; m_difG = g; m_difB = b; }
    void SetSpecular(float r, float g, float b) { m_specR = r; m_specG = g; m_specB = b; }

    float GetPosX() const { return m_posX; }
    float GetPosY() const { return m_posY; }
    float GetPosZ() const { return m_posZ; }
    float GetAmbientR() const { return m_ambR; }
    float GetAmbientG() const { return m_ambG; }
    float GetAmbientB() const { return m_ambB; }
    float GetDiffuseR() const { return m_difR; }
    float GetDiffuseG() const { return m_difG; }
    float GetDiffuseB() const { return m_difB; }
    float GetSpecularR() const { return m_specR; }
    float GetSpecularG() const { return m_specG; }
    float GetSpecularB() const { return m_specB; }

private:
    float m_posX, m_posY, m_posZ;
    float m_ambR, m_ambG, m_ambB;
    float m_difR, m_difG, m_difB;
    float m_specR, m_specG, m_specB;
};
