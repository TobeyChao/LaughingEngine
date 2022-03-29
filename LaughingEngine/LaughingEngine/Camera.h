#pragma once
#include "PCH.h"
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
	// λ��
	inline XMVECTOR GetPosition() const
	{
		return XMLoadFloat3(&mPosition);
	}
	inline XMFLOAT3 GetPosition3f() const
	{
		return mPosition;
	}
	inline void SetPosition(float x, float y, float z)
	{
		mPosition = XMFLOAT3(x, y, z);
	}
	inline void SetPosition3f(const XMFLOAT3& pos)
	{
		mPosition = pos;
	}

	// ������
	XMVECTOR GetRight() const
	{
		return XMLoadFloat3(&mRight);
	}
	XMFLOAT3 GetRight3f() const
	{
		return mRight;
	}
	XMVECTOR GetUp() const
	{
		return XMLoadFloat3(&mUp);
	}
	XMFLOAT3 GetUp3f() const
	{
		return mUp;
	}
	XMVECTOR GetLook() const
	{
		return XMLoadFloat3(&mForward);
	}
	XMFLOAT3 GetLook3f() const
	{
		return mForward;
	}

	// ��׶������
	float GetNearZ() const
	{
		return mNearZ;
	}
	float GetFarZ() const
	{
		return mFarZ;
	}
	float GetAspect() const
	{
		return mAspect;
	}
	float GetFovY() const
	{
		return mFovY;
	}
	float GetFovX() const
	{
		return 0.0f;
	}

	// ��ȡ��Զ�ü���Ĵ�С
	float GetNearWindowWidth() const
	{
		return 0.0f;
	}
	float GetNearWindowHeight() const
	{
		return 0.0f;
	}
	float GetFarWindowWidth() const
	{
		return 0.0f;
	}
	float GetFarWindowHeight() const
	{
		return 0.0f;
	}

	// ����
	inline const DirectX::XMMATRIX& GetViewMatrix() const
	{
		return mView;
	}
	inline const DirectX::XMMATRIX& GetProjMatrix() const
	{
		return mProj;
	}

	// ������׶��
	void SetLens(float fov, float aspect, float zn, float zf);

	// �ƶ������
	void Walk(float d);
	void Strafe(float d);

	// ��ת�����
	void Pitch(float pitch);
	void Yaw(float yaw);
	void Roll(float roll);

	void ComputeInfo();

private:
	bool mDirty = true;

	float mFovY;
	float mAspect;
	float mNearZ;
	float mFarZ;

	float mPitch = 0;
	float mYaw = 0;
	float mRoll = 0;

	// Camera coordinate system with coordinates relative to world space.
	DirectX::XMFLOAT3 mPosition = { 0.0f, 5.0f, 0.0f };

	DirectX::XMFLOAT3 mRight = { 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 mUp = { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 mForward = { 0.0f, 0.0f, 1.0f };

	DirectX::XMVECTOR mDefaultRight = { 1.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMVECTOR mDefaultUp = { 0.0f, 1.0f, 0.0f, 1.0f };
	DirectX::XMVECTOR mDefaultForward = { 0.0f, 0.0f, 1.0f, 1.0f };

	DirectX::XMMATRIX mProj;
	DirectX::XMMATRIX mView;
};