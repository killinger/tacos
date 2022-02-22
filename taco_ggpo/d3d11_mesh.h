#pragma once
#include <d3d11.h>
#include "vertex.h"

struct d3d11_mesh
{
	ID3D11Buffer*	m_IndexBuffer;
	ID3D11Buffer*	m_VertexBuffer;
	uint32			m_IndexEntryCount;
	uint32			m_VertexCount;

	void CreateIndexBuffer(ID3D11Device* Device, uint32* Indices)
	{
		D3D11_BUFFER_DESC IndexBufferDescription = { 0 };
		IndexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		IndexBufferDescription.ByteWidth = sizeof(uint32) * m_IndexEntryCount;
		IndexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
		IndexBufferDescription.CPUAccessFlags = 0;
		IndexBufferDescription.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA IndexBufferData = { 0 };
		IndexBufferData.pSysMem = Indices;
		HRESULT Result = Device->CreateBuffer(&IndexBufferDescription, &IndexBufferData, &m_IndexBuffer);
	}

	void CreateVertexBuffer(ID3D11Device* Device, vertex_3D* Vertices)
	{
		D3D11_BUFFER_DESC VertexBufferDescription = { 0 };
		VertexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		VertexBufferDescription.ByteWidth = sizeof(vertex_3D) * m_VertexCount;
		VertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		VertexBufferDescription.CPUAccessFlags = 0;
		VertexBufferDescription.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA VertexBufferData = { 0 };
		VertexBufferData.pSysMem = Vertices;
		HRESULT Result = Device->CreateBuffer(&VertexBufferDescription, &VertexBufferData, &m_VertexBuffer);
	}
};