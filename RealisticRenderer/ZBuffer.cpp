#include "stdafx.h"
#include "ZBuffer.h"
#include <cstring>

CZBuffer::CZBuffer() : m_buffer(nullptr), m_width(0), m_height(0) {}

CZBuffer::~CZBuffer() { delete[] m_buffer; }

void CZBuffer::Init(int width, int height) {
    delete[] m_buffer;
    m_width = width;
    m_height = height;
    m_buffer = new float[width * height];
}

void CZBuffer::Clear(float depth) {
    if (m_buffer) {
        for (int i = 0; i < m_width * m_height; i++)
            m_buffer[i] = depth;
    }
}

bool CZBuffer::TestAndSet(int x, int y, float depth) {
    if (!IsValid(x, y)) return false;
    int idx = y * m_width + x;
    if (depth < m_buffer[idx]) {
        m_buffer[idx] = depth;
        return true;
    }
    return false;
}

float CZBuffer::GetDepth(int x, int y) const {
    if (!IsValid(x, y)) return 1.0f;
    return m_buffer[y * m_width + x];
}
