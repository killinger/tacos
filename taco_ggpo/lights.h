#pragma once

struct directional_light
{
	DirectX::XMFLOAT3	Direction;
	float				Padding;
	DirectX::XMFLOAT4	Ambient;
	DirectX::XMFLOAT4	Diffuse;
};

struct point_light
{
	DirectX::XMFLOAT4	Ambient;
	DirectX::XMFLOAT4	Diffuse;
	DirectX::XMFLOAT3	Position;
	float				Padding;
	float				Attentuation;
	float				Padding2[3];
};