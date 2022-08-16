#pragma once
#include "ConstantBuffer.h"
#include "../Core/pch.h"

class Camera
{
public:
    Camera() {}
    Camera(Camera* other): position(other->position), focus(other->focus), up(other->up){}
    
    XMVECTOR position = XMVectorSet(0.0f, 3.0f, -3.0f, 0.0f);
    XMVECTOR focus = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    static UINT GetConstantSize()
    {
        return sizeof(XMMATRIX); // view + proj
    }

    XMMATRIX GetViewProjectionMatrix(LONG screenWidth, LONG screenHeight, FLOAT fovInDegrees = 60)
    {
        
        float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
        float fovAngleY = fovInDegrees * XM_PI / 180.0f;

        if (aspectRatio < 1.0f)
        {
            fovAngleY /= aspectRatio;
        }
        auto view = XMMatrixLookAtLH(position, focus, up);
        auto proj = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, 0.01f, 1000.0f);
        return XMMatrixTranspose(XMMatrixMultiply(view, proj));
    }

};
