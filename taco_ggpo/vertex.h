#pragma once
#include <DirectXMath.h>

struct vertex_3D
{
	vertex_3D() :
		Position(0.0f, 0.0f, 0.0f),
		TextureCoord(0.0f, 0.0f),
		Normal(0.0f, 0.0f, 0.0f) { }
	vertex_3D(	float X, float Y, float Z, 
			float U, float V) : 
		Position(X, Y, Z),
		TextureCoord(U, V),
		Normal(0.0f, 0.0f, 0.0f) { }
	vertex_3D(	float PX, float PY, float PZ,
			float U, float V,
			float NX, float NY, float NZ) :
		Position(PX, PY, PZ),
		TextureCoord(U, V),
		Normal(NX, NY, NZ) { }
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 TextureCoord;
	DirectX::XMFLOAT3 Normal;
};

struct vertex_2D
{
	vertex_2D(float X, float Y, float Z,
		float U, float V) :
		Position(X, Y, Z),
		TextureCoord(U, V) {}
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 TextureCoord;
};