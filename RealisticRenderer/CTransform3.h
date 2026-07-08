#pragma once
#include "CP3.h"

class CTransform3 {
public:
    CTransform3();
    void Identity();
    void Translate(double tx, double ty, double tz);
    void Scale(double sx, double sy, double sz);
    void RotateX(double angle);
    void RotateY(double angle);
    void RotateZ(double angle);
    void MultiplyMatrix(CP3* P, int ptNumber);

private:
    double T[4][4];
};
