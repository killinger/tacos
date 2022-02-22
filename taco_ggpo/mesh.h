#pragma once
#include "vertex.h"
#include "defs.h"

struct face
{
	uint32 Indices[3];
};

struct mesh
{
	vertex_3D*	Vertices;
	face*	Faces;
	uint32	FaceCount;
	uint32	VertexCount;
};