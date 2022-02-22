#pragma once
#include <DirectXMath.h>
#include "defs.h"
#include "d3d11_mesh.h"
#include "lights.h"

class camera;
struct gamestate;

using namespace DirectX;

class character_node
{
public:
	struct offscreen_transforms
	{
		XMMATRIX m_WorldTransform;
		XMMATRIX m_MVPTransform;
	};
	struct character_lights
	{
		directional_light	m_DirectionalLight;
		point_light			m_PointLight;
	};
	struct outline_properties
	{
		XMFLOAT4	m_Color;
		float		m_Thickness;
		float		Padding[3];
	};
	offscreen_transforms	m_OffscreenTransforms;
	character_lights		m_OffscreenLights;
	outline_properties		m_OutlineProperties;
	XMMATRIX				m_WorldTransform;
	d3d11_mesh				m_Mesh;
};

class scene_graph
{
public:
	scene_graph();

	camera*			m_Camera;
	character_node	m_CharacterNodes[2];

	void Initialize();
	void Update(gamestate* GameState);
};