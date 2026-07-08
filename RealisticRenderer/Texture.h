#pragma once

class CTexture {
public:
    CTexture();
    ~CTexture();

    bool Load(const wchar_t* filename);
    bool IsLoaded() const { return m_data != nullptr; }

    // Bilinear sample at UV coordinates, returns RGB [0-255]
    void Sample(float u, float v, unsigned char& r, unsigned char& g, unsigned char& b) const;

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    unsigned char* m_data; // RGBA pixels
    int m_width, m_height;
};
