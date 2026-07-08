#include "pch.h"
#include "CTransform3.h"

CTransform3::CTransform3() { Identity(); }

void CTransform3::Identity() {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            T[i][j] = (i == j) ? 1.0 : 0.0;
}

void CTransform3::Translate(double tx, double ty, double tz) {
    Identity();
    T[3][0] = tx; T[3][1] = ty; T[3][2] = tz;
}

void CTransform3::Scale(double sx, double sy, double sz) {
    Identity();
    T[0][0] = sx; T[1][1] = sy; T[2][2] = sz;
}

void CTransform3::RotateX(double angle) {
    Identity();
    double c = cos(angle), s = sin(angle);
    T[1][1] = c;  T[1][2] = s;
    T[2][1] = -s; T[2][2] = c;
}

void CTransform3::RotateY(double angle) {
    Identity();
    double c = cos(angle), s = sin(angle);
    T[0][0] = c;  T[0][2] = -s;
    T[2][0] = s;  T[2][2] = c;
}

void CTransform3::RotateZ(double angle) {
    Identity();
    double c = cos(angle), s = sin(angle);
    T[0][0] = c;  T[0][1] = s;
    T[1][0] = -s; T[1][1] = c;
}

void CTransform3::MultiplyMatrix(CP3* P, int ptNumber) {
    for (int k = 0; k < ptNumber; k++) {
        double x = P[k].x * T[0][0] + P[k].y * T[1][0] + P[k].z * T[2][0] + P[k].w * T[3][0];
        double y = P[k].x * T[0][1] + P[k].y * T[1][1] + P[k].z * T[2][1] + P[k].w * T[3][1];
        double z = P[k].x * T[0][2] + P[k].y * T[1][2] + P[k].z * T[2][2] + P[k].w * T[3][2];
        double w = P[k].x * T[0][3] + P[k].y * T[1][3] + P[k].z * T[2][3] + P[k].w * T[3][3];
        P[k].x = x; P[k].y = y; P[k].z = z; P[k].w = w;
    }
}
