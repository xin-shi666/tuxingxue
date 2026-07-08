#pragma once

class CTexture {
public:
    CTexture();
    ~CTexture();

    BOOL Load(const wchar_t* filename);
    BOOL IsLoaded() const { return m_loaded; }

    // Bilinear sample at UV coordinates
    CRGB Sample(double u, double v) const;

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    unsigned char* m_data;
    int m_width, m_height;
    BOOL m_loaded;
};
