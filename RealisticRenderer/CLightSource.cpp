#include "pch.h"
#include "CLightSource.h"

CLightSource::CLightSource() {
    L_Diffuse = CRGB(1, 1, 1);
    L_Specular = CRGB(1, 1, 1);
    L_Position = CP3(300, 300, 500);
    L_C0 = 1.0; L_C1 = 0; L_C2 = 0;
    L_OnOff = TRUE;
}
