#include "pch.h"
#include "CTexture.h"
#include "CRGB.h"

CTexture::CTexture() : m_data(nullptr), m_width(0), m_height(0), m_loaded(FALSE) {}

CTexture::~CTexture() { delete[] m_data; }

BOOL CTexture::Load(const wchar_t* filename) {
    delete[] m_data;
    m_data = nullptr;
    m_loaded = FALSE;

    Gdiplus::Bitmap bitmap(filename);
    if (bitmap.GetLastStatus() != Gdiplus::Ok) return FALSE;

    m_width = bitmap.GetWidth();
    m_height = bitmap.GetHeight();
    m_data = new unsigned char[m_width * m_height * 4];

    Gdiplus::BitmapData bmData;
    Gdiplus::Rect rect(0, 0, m_width, m_height);
    bitmap.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmData);
    memcpy(m_data, bmData.Scan0, m_width * m_height * 4);
    bitmap.UnlockBits(&bmData);

    m_loaded = TRUE;
    return TRUE;
}

CRGB CTexture::Sample(double u, double v) const {
    if (!m_loaded) return CRGB(0.5, 0.5, 0.5);

    // Wrap addressing
    u = u - floor(u);
    v = v - floor(v);

    double fx = u * m_width - 0.5;
    double fy = v * m_height - 0.5;
    int x0 = (int)floor(fx), y0 = (int)floor(fy);
    int x1 = x0 + 1, y1 = y0 + 1;
    double sx = fx - x0, sy = fy - y0;

    // Wrap
    x0 = (x0 % m_width + m_width) % m_width;
    x1 = (x1 % m_width + m_width) % m_width;
    y0 = (y0 % m_height + m_height) % m_height;
    y1 = (y1 % m_height + m_height) % m_height;

    auto getPixel = [this](int x, int y, int ch) -> double {
        return m_data[(y * m_width + x) * 4 + ch] / 255.0;
    };

    double r0 = getPixel(x0, y0, 2) + (getPixel(x1, y0, 2) - getPixel(x0, y0, 2)) * sx;
    double r1 = getPixel(x0, y1, 2) + (getPixel(x1, y1, 2) - getPixel(x0, y1, 2)) * sx;
    double g0 = getPixel(x0, y0, 1) + (getPixel(x1, y0, 1) - getPixel(x0, y0, 1)) * sx;
    double g1 = getPixel(x0, y1, 1) + (getPixel(x1, y1, 1) - getPixel(x0, y1, 1)) * sx;
    double b0 = getPixel(x0, y0, 0) + (getPixel(x1, y0, 0) - getPixel(x0, y0, 0)) * sx;
    double b1 = getPixel(x0, y1, 0) + (getPixel(x1, y1, 0) - getPixel(x0, y1, 0)) * sx;

    return CRGB(r0 + (r1 - r0) * sy, g0 + (g1 - g0) * sy, b0 + (b1 - b0) * sy);
}
