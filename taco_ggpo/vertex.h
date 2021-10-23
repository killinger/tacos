#pragma once
#include <DirectXMath.h>

struct vertex
{
	vertex() {}
	vertex(	float X, float Y, float Z, 
			float U, float V) : 
		Position(X, Y, Z),
		TextureCoord(U, V) { }
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 TextureCoord;
};