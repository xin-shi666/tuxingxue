#include "stdafx.h"
#include "Texture.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

CTexture::CTexture() : m_data(nullptr), m_width(0), m_height(0) {}

CTexture::~CTexture() { delete[] m_data; }

bool CTexture::Load(const wchar_t* filename) {
    delete[] m_data;
    m_data = nullptr;

    Gdiplus::Bitmap bitmap(filename);
    if (bitmap.GetLastStatus() != Gdiplus::Ok) return false;

    m_width = bitmap.GetWidth();
    m_height = bitmap.GetHeight();
    m_data = new unsigned char[m_width * m_height * 4];

    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            Gdiplus::Color color;
            bitmap.GetPixel(x, y, &color);
            int idx = (y * m_width + x) * 4;
            m_data[idx + 0] = color.GetR();
            m_data[idx + 1] = color.GetG();
            m_data[idx + 2] = color.GetB();
            m_data[idx + 3] = color.GetA();
        }
    }
    return true;
}

void CTexture::Sample(float u, float v, unsigned char& r, unsigned char& g, unsigned char& b) const {
    if (!m_data) { r = g = b = 128; return; }

    // Wrap addressing
    u = u - floorf(u);
    v = v - floorf(v);

    float fx = u * m_width - 0.5f;
    float fy = v * m_height - 0.5f;

    int x0 = (int)floorf(fx);
    int y0 = (int)floorf(fy);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float sx = fx - x0;
    float sy = fy - y0;

    // Wrap coordinates
    auto wrap = [](int& val, int max) {
        val = val % max;
        if (val < 0) val += max;
    };
    wrap(x0, m_width); wrap(x1, m_width);
    wrap(y0, m_height); wrap(y1, m_height);

    auto samplePixel = [this](int x, int y, int ch) -> float {
        return m_data[(y * m_width + x) * 4 + ch];
    };

    // Bilinear interpolation
    float r00 = samplePixel(x0, y0, 0), g00 = samplePixel(x0, y0, 1), b00 = samplePixel(x0, y0, 2);
    float r10 = samplePixel(x1, y0, 0), g10 = samplePixel(x1, y0, 1), b10 = samplePixel(x1, y0, 2);
    float r01 = samplePixel(x0, y1, 0), g01 = samplePixel(x0, y1, 1), b01 = samplePixel(x0, y1, 2);
    float r11 = samplePixel(x1, y1, 0), g11 = samplePixel(x1, y1, 1), b11 = samplePixel(x1, y1, 2);

    float r0 = r00 + (r10 - r00) * sx;
    float r1 = r01 + (r11 - r01) * sx;
    float g0 = g00 + (g10 - g00) * sx;
    float g1 = g01 + (g11 - g01) * sx;
    float b0 = b00 + (b10 - b00) * sx;
    float b1 = b01 + (b11 - b01) * sx;

    r = (unsigned char)(r0 + (r1 - r0) * sy);
    g = (unsigned char)(g0 + (g1 - g0) * sy);
    b = (unsigned char)(b0 + (b1 - b0) * sy);
}
