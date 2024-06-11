#include "Frustum.h"
#include "Camera.h"

CCamera* Camera;

void Frustum::FinalUpdate()
{
	XMFLOAT4X4 matViewInv = Matrix4x4::Inverse(Camera->GetViewMatrix());
	XMFLOAT4X4 matProjectionInv = Matrix4x4::Inverse(Camera->GetProjectionMatrix());
	XMFLOAT4X4 matInv = Matrix4x4::Multiply(matProjectionInv, matViewInv);

	vector<XMVECTOR> worldPos =
	{
		XMVector3TransformCoord(XMVECTOR{ -1.f, 1.f, 0.f},XMLoadFloat4x4(&matInv)),
		XMVector3TransformCoord(XMVECTOR{1.f, 1.f, 0.f}, XMLoadFloat4x4(&matInv)),
		XMVector3TransformCoord(XMVECTOR{1.f, -1.f, 0.f }, XMLoadFloat4x4(&matInv)),
		XMVector3TransformCoord(XMVECTOR{-1.f, -1.f, 0.f}, XMLoadFloat4x4(&matInv)),
		XMVector3TransformCoord(XMVECTOR{-1.f, 1.f, 1.f}, XMLoadFloat4x4(&matInv)),
		XMVector3TransformCoord(XMVECTOR{1.f, 1.f, 1.f}, XMLoadFloat4x4(&matInv)),
		XMVector3TransformCoord(XMVECTOR{1.f, -1.f, 1.f}, XMLoadFloat4x4(&matInv)),
		XMVector3TransformCoord(XMVECTOR{-1.f, -1.f, 1.f}, XMLoadFloat4x4(&matInv))
	};

	_planes[PLANE_FRONT] = XMFLOAT4(
		Vector3::XMVectorToFloat3(XMPlaneFromPoints(worldPos[0], worldPos[1], worldPos[2])).x,
		Vector3::XMVectorToFloat3(XMPlaneFromPoints(worldPos[0], worldPos[1], worldPos[2])).y,
		Vector3::XMVectorToFloat3(XMPlaneFromPoints(worldPos[0], worldPos[1], worldPos[2])).z,
		1); // CW
	_planes[PLANE_BACK] = XMFLOAT4(
		Vector3::XMVectorToFloat3(XMPlaneFromPoints(worldPos[4], worldPos[7], worldPos[5])).x,
		Vector3::XMVectorToFloat3(XMPlaneFromPoints(worldPos[4], worldPos[7], worldPos[5])).y,
		Vector3::XMVectorToFloat3(XMPlaneFromPoints(worldPos[4], worldPos[7], worldPos[5])).z,
		1);
	_planes[PLANE_UP] = XMFLOAT4(
		Vector3::XMVectorToFloat3(XMPlaneFromPoints(worldPos[4], worldPos[5], worldPos[1])).x,
		Vector3::XMVectorToFloat3(XMPlaneFromPoints(worldPos[4], worldPos[5], worldPos[1])).y,
		Vector3::XMVectorToFloat3(XMPlaneFromPoints(worldPos[4], worldPos[5], worldPos[1])).z,
		1);
	_planes[PLANE_DOWN] = XMFLOAT4(
		Vector3::XMVectorToFloat3(XMPlaneFromPoints(worldPos[7], worldPos[3], worldPos[6])).x,
		Vector3::XMVectorToFloat3(XMPlaneFromPoints(worldPos[7], worldPos[3], worldPos[6])).y,
		Vector3::XMVectorToFloat3(XMPlaneFromPoints(worldPos[7], worldPos[3], worldPos[6])).z,
		1);
	_planes[PLANE_LEFT] = XMFLOAT4(
		Vector3::XMVectorToFloat3(XMPlaneFromPoints(worldPos[4], worldPos[0], worldPos[7])).x,
		Vector3::XMVectorToFloat3(XMPlaneFromPoints(worldPos[4], worldPos[0], worldPos[7])).y,
		Vector3::XMVectorToFloat3(XMPlaneFromPoints(worldPos[4], worldPos[0], worldPos[7])).z,
		1);
	_planes[PLANE_RIGHT] = XMFLOAT4(
		Vector3::XMVectorToFloat3(XMPlaneFromPoints(worldPos[5], worldPos[6], worldPos[1])).x,
		Vector3::XMVectorToFloat3(XMPlaneFromPoints(worldPos[5], worldPos[6], worldPos[1])).y,
		Vector3::XMVectorToFloat3(XMPlaneFromPoints(worldPos[5], worldPos[6], worldPos[1])).z,
		1);
}

bool Frustum::ContainsSphere(XMFLOAT3& pos, float radius)
{
	for (const XMFLOAT4& plane : _planes)
	{
		// n = (a, b, c)
		XMFLOAT3 normal = XMFLOAT3(plane.x, plane.y, plane.z);

		// ax + by + cz + d > radius
		if (Vector3::DotProduct(normal,pos) + plane.w > radius)
			return false;
	}

	return true;
}