#include "stdafx.h"
#include "LightSource.h"

CLightSource::CLightSource() {
    m_posX = 5.0f; m_posY = 5.0f; m_posZ = 5.0f;
    m_ambR = 0.2f; m_ambG = 0.2f; m_ambB = 0.2f;
    m_difR = 0.8f; m_difG = 0.8f; m_difB = 0.8f;
    m_specR = 1.0f; m_specG = 1.0f; m_specB = 1.0f;
}
