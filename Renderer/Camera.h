#pragma once
#include "ConstantBuffer.h"
#include "../Core/pch.h"

class Camera
{
	
	XMFLOAT3 position = {0.0f, 0.0f, 0.0f};
	XMFLOAT3 focus = {0.0f, 0.0f, 0.0f};
	XMFLOAT3 right = {1.0f, 0.0f, 0.0f};
	LONG screenWidth = 0;
	LONG screenHeight = 0;
	FLOAT fovInDegrees = 45;
	FLOAT nearZ = 1.0f;
	FLOAT farZ = 1000.0f;
	XMMATRIX viewMatrix = {};
	XMMATRIX projectionMatrix = {};
	BOOL dirty = TRUE;

public:
    Camera() {}
    Camera(Camera* other): position(other->position), focus(other->focus), screenWidth(other->screenWidth), screenHeight(other->screenHeight){}

	static UINT GetConstantSize()
	{
		return sizeof(XMMATRIX); // view + proj
	}

	XMVECTOR GetPosition()
	{
		return XMLoadFloat3(&position);
	}

	FLOAT GetNearZ()
	{
		return nearZ;
	}
	
	FLOAT GetFarZ()
	{
		return farZ;
	}
	
	LONG GetWidth()
	{
		return screenWidth;
	}
	
	LONG GetHeight()
	{
		return screenHeight;
	}
	
	void UpdateScreenSize(LONG width, LONG height)
	{
		screenWidth = width;
		screenHeight = height;
	}

	void LookAt(XMVECTOR worldPosition, XMVECTOR target, XMVECTOR worldRight)
	{
		XMStoreFloat3(&position, worldPosition);
		XMStoreFloat3(&focus, target);
		XMStoreFloat3(&right, worldRight);
		dirty = TRUE;
	}

    XMMATRIX GetViewMatrix()
    {
		if (dirty)
		{
			XMVECTOR R = XMLoadFloat3(&right);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR L = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&focus), XMLoadFloat3(&position)));

			// Keep camera's axes orthogonal to each other and of unit length.
			XMVECTOR U = XMVector3Normalize(XMVector3Cross(L, R));

			// U, L already ortho-normal, so no need to normalize cross product.
			// R = XMVector3Cross(U, L);

			// Fill in the view matrix entries.
			float x = -XMVectorGetX(XMVector3Dot(P, R));
			float y = -XMVectorGetX(XMVector3Dot(P, U));
			float z = -XMVectorGetX(XMVector3Dot(P, L));
			XMFLOAT4X4 mView = {};
			mView(0, 0) = right.x;
			mView(1, 0) = right.y;
			mView(2, 0) = right.z;
			mView(3, 0) = x;

			mView(0, 1) = U.m128_f32[0];
			mView(1, 1) = U.m128_f32[1];
			mView(2, 1) = U.m128_f32[2];
			mView(3, 1) = y;

			mView(0, 2) = L.m128_f32[0];
			mView(1, 2) = L.m128_f32[1];
			mView(2, 2) = L.m128_f32[2];
			mView(3, 2) = z;

			mView(0, 3) = 0.0f;
			mView(1, 3) = 0.0f;
			mView(2, 3) = 0.0f;
			mView(3, 3) = 1.0f;
			viewMatrix = XMLoadFloat4x4(&mView);
			dirty = FALSE;
		}
        return viewMatrix;
    }

    XMMATRIX GetProjectionMatrix()
    {
        float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
        float fovAngleY = fovInDegrees * XM_PI / 180.0f;

        if (aspectRatio < 1.0f)
        {
            fovAngleY /= aspectRatio;
        }
        return XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);

    }

    XMMATRIX GetViewProjectionMatrix()
    {
        auto view = GetViewMatrix();
        auto proj = GetProjectionMatrix();
	   return XMMatrixMultiply(view, proj);
    }

    XMMATRIX GetProjectionToWorldMatrix()
	{
    	auto viewProj = GetViewProjectionMatrix();
		XMVECTOR deter = XMMatrixDeterminant(viewProj);
		return XMMatrixInverse(&deter, viewProj);
    }

};

