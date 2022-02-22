#include "camera.h"
#include <stdlib.h>

camera::camera()
{
	m_Position.x = 0.0f;
	m_Position.y = 1.75;
	m_Position.z = -7.0f;
	m_Velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	m_Friction = XMVectorSet(0.19f, 0.19f, 0.19f, 0.0f);
	m_Target = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	m_UpDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_FOV = 59.0f;
}

void camera::Update(gamestate* Gamestate, uint32 Inputs)
{
	XMFLOAT3 NewAcceleration(0.0f, 0.0f, 0.0f);
	if (Inputs & INPUT_LEFT)
		NewAcceleration.x = -1.0f;
	else if (Inputs & INPUT_RIGHT)
		NewAcceleration.x = 1.0f;
	if (Inputs & INPUT_UP)
		NewAcceleration.y = 1.0f;
	else if (Inputs & INPUT_DOWN)
		NewAcceleration.y = -1.0f;
	if (Inputs & INPUT_A)
		NewAcceleration.z = 1.0f;
	else if (Inputs & INPUT_D)
		NewAcceleration.z = -1.0f;

	XMVECTOR NormalizedAccel = XMLoadFloat3(&NewAcceleration);
	NormalizedAccel = XMVector3Normalize(NormalizedAccel);

	m_Velocity += (NormalizedAccel * 0.15f);
	m_Velocity -= (m_Friction * m_Velocity);
	XMVECTOR VelocityLength = XMVector3Length(m_Velocity);
	XMFLOAT3 VelocityFloat3(0.0f, 0.0f, 0.0f);
	XMStoreFloat3(&VelocityFloat3, VelocityLength);
	if (VelocityFloat3.x < 0.001f)
		m_Velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	XMVECTOR NewPosition = XMLoadFloat3((const XMFLOAT3*)&m_Position);
	NewPosition += m_Velocity;
	XMStoreFloat3(&m_Position, NewPosition);
}
