#pragma once

class CZBuffer {
public:
    CZBuffer();
    ~CZBuffer();
    void Init(int width, int height);
    void Clear(float depth = 1.0f);
    bool TestAndSet(int x, int y, float depth);
    float GetDepth(int x, int y) const;
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    bool IsValid(int x, int y) const { return x >= 0 && x < m_width && y >= 0 && y < m_height; }

private:
    float* m_buffer;
    int m_width, m_height;
};
