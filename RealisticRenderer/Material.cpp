#include "stdafx.h"
#include "Material.h"

CMaterial::CMaterial() {
    m_kaR = 0.2f; m_kaG = 0.2f; m_kaB = 0.2f;
    m_kdR = 0.8f; m_kdG = 0.8f; m_kdB = 0.8f;
    m_ksR = 0.5f; m_ksG = 0.5f; m_ksB = 0.5f;
    m_shininess = 32.0f;
}
