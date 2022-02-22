#pragma once
#include "defs.h"
#include "gamestate.h"
#include <DirectXMath.h>

#define BOUND_LEFT_MAX -640.0f
#define BOUND_RIGHT_MAX 640.0f

using namespace DirectX;

class camera
{
public:
	camera();
	
	//sf::View	m_View;
	//float		m_LeftBound;
	//float		m_RightBound;
	XMVECTOR	m_Target;
	XMVECTOR	m_UpDirection;
	XMVECTOR	m_Velocity;
	XMVECTOR	m_Friction;
	XMFLOAT3	m_Position;
	float		m_FOV;
	void		Update(gamestate* Gamestate, uint32 Inputs);
private:

};

