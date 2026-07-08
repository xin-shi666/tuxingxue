#pragma once

class CMaterial {
public:
    CMaterial();

    void SetAmbient(float r, float g, float b) { m_kaR = r; m_kaG = g; m_kaB = b; }
    void SetDiffuse(float r, float g, float b) { m_kdR = r; m_kdG = g; m_kdB = b; }
    void SetSpecular(float r, float g, float b) { m_ksR = r; m_ksG = g; m_ksB = b; }
    void SetShininess(float s) { m_shininess = s; }

    float GetKaR() const { return m_kaR; } float GetKaG() const { return m_kaG; } float GetKaB() const { return m_kaB; }
    float GetKdR() const { return m_kdR; } float GetKdG() const { return m_kdG; } float GetKdB() const { return m_kdB; }
    float GetKsR() const { return m_ksR; } float GetKsG() const { return m_ksG; } float GetKsB() const { return m_ksB; }
    float GetShininess() const { return m_shininess; }

private:
    float m_kaR, m_kaG, m_kaB;   // ambient
    float m_kdR, m_kdG, m_kdB;   // diffuse
    float m_ksR, m_ksG, m_ksB;   // specular
    float m_shininess;
};
