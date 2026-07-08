#pragma once

class CRGB {
public:
    double r, g, b;
    CRGB() : r(0), g(0), b(0) {}
    CRGB(double _r, double _g, double _b) : r(_r), g(_g), b(_b) {}

    CRGB operator+(const CRGB& c) const { return CRGB(r + c.r, g + c.g, b + c.b); }
    CRGB operator-(const CRGB& c) const { return CRGB(r - c.r, g - c.g, b - c.b); }
    CRGB operator*(double s) const { return CRGB(r * s, g * s, b * s); }
    CRGB operator*(const CRGB& c) const { return CRGB(r * c.r, g * c.g, b * c.b); }
    CRGB& operator+=(const CRGB& c) { r += c.r; g += c.g; b += c.b; return *this; }

    void Normalize() {
        if (r < 0) r = 0; if (r > 1) r = 1;
        if (g < 0) g = 0; if (g > 1) g = 1;
        if (b < 0) b = 0; if (b > 1) b = 1;
    }

    COLORREF ToCOLORREF() const {
        int ir = (int)(r * 255); if (ir < 0) ir = 0; if (ir > 255) ir = 255;
        int ig = (int)(g * 255); if (ig < 0) ig = 0; if (ig > 255) ig = 255;
        int ib = (int)(b * 255); if (ib < 0) ib = 0; if (ib > 255) ib = 255;
        return RGB(ir, ig, ib);
    }
};
