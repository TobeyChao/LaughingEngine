#include "PCH.h"
#include "Camera.h"

void Camera::SetLens(float fov, float aspect, float zn, float zf)
{
	mFovY = fov;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mProj = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
}

void Camera::Walk(float d)
{
	XMStoreFloat3(&mPosition, XMLoadFloat3(&mPosition) + XMLoadFloat3(&mForward) * d);
	mDirty = true;
}

void Camera::Strafe(float d)
{
	XMStoreFloat3(&mPosition, XMLoadFloat3(&mPosition) + XMLoadFloat3(&mRight) * d);
	mDirty = true;
}

void Camera::Pitch(float pitch)
{
	if (mPitch == pitch)
	{
		return;
	}
	mPitch = pitch;
	mDirty = true;
}

void Camera::Yaw(float yaw)
{
	if (mYaw == yaw)
	{
		return;
	}
	mYaw = yaw;
	mDirty = true;
}

void Camera::Roll(float roll)
{
	if (mRoll == roll)
	{
		return;
	}
	mRoll = roll;
	mDirty = true;
}

void Camera::ComputeInfo()
{
	if (!mDirty)
	{
		return;
	}

	// 这里默认以：
	// z轴正方向为forward
	// y轴正方向为up
	// x轴正方向为right
	auto rotateMat = DirectX::XMMatrixRotationRollPitchYaw(mPitch, mYaw, mRoll);
	XMStoreFloat3(&mForward, DirectX::XMVector3TransformNormal(mDefaultForward, rotateMat));
	XMStoreFloat3(&mUp, DirectX::XMVector3TransformNormal(mDefaultUp, rotateMat));
	XMStoreFloat3(&mRight, DirectX::XMVector3Cross(XMLoadFloat3(&mUp), XMLoadFloat3(&mForward)));
	mView = DirectX::XMMatrixLookToLH(XMLoadFloat3(&mPosition), XMLoadFloat3(&mForward), XMLoadFloat3(&mUp));

	mDirty = false;
}