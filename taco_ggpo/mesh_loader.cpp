#include "mesh_loader.h"
#include <assimp\cimport.h>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

mesh assets::LoadMeshFromFile(const char* Filename)
{
	const aiScene* Scene = aiImportFile(Filename, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate);
	const aiMesh* Mesh = Scene->mMeshes[0];
	mesh Result = { 0 };
	Result.VertexCount = Mesh->mNumVertices;
	Result.Vertices = new vertex_3D[Result.VertexCount];
	for (uint32 i = 0; i < Result.VertexCount; i++)
	{
		Result.Vertices[i].Position.x = Mesh->mVertices[i].x;
		Result.Vertices[i].Position.y = Mesh->mVertices[i].y;
		Result.Vertices[i].Position.z = Mesh->mVertices[i].z;
		Result.Vertices[i].Normal.x = Mesh->mNormals[i].x;
		Result.Vertices[i].Normal.y = Mesh->mNormals[i].y;
		Result.Vertices[i].Normal.z = Mesh->mNormals[i].z;
		Result.Vertices[i].TextureCoord.x = Mesh->mTextureCoords[0][i].x;
		Result.Vertices[i].TextureCoord.y = 1.0f - Mesh->mTextureCoords[0][i].y;
	}

	Result.FaceCount = Mesh->mNumFaces;
	Result.Faces = new face[Result.FaceCount];
	aiFace* Face = Mesh->mFaces;
	for (uint32 i = 0; i < Result.FaceCount; i++)
	{
		Result.Faces[i].Indices[0] = Face[i].mIndices[0];
		Result.Faces[i].Indices[1] = Face[i].mIndices[1];
		Result.Faces[i].Indices[2] = Face[i].mIndices[2];
	}
	aiReleaseImport(Scene);
	return Result;
}