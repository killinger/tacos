#pragma once

struct test_mesh
{
	ID3D11Buffer*	m_IndexBuffer;
	ID3D11Buffer*	m_VertexBuffer;
	uint32			m_IndexEntryCount;
	uint32			m_VertexCount;
	void Initialize(ID3D11Device* Device, uint32* Indices, uint32 IndexEntryCount, vertex_3D* Vertices, uint32 VertexCount)
	{
		/*m_IndexEntryCount = IndexEntryCount;
		m_VertexCount = VertexCount;*/

		mesh Mesh = assets::LoadMeshFromFile("data/monk2.dae");
		m_IndexEntryCount = Mesh.FaceCount * 3;
		m_VertexCount = Mesh.VertexCount;

		D3D11_BUFFER_DESC IndexBufferDescription = { 0 };
		IndexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		IndexBufferDescription.ByteWidth = sizeof(uint32) * m_IndexEntryCount;
		IndexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
		IndexBufferDescription.CPUAccessFlags = 0;
		IndexBufferDescription.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA IndexBufferData = { 0 };
		IndexBufferData.pSysMem = Mesh.Faces;
		HRESULT Result = Device->CreateBuffer(&IndexBufferDescription, &IndexBufferData, &m_IndexBuffer);

		CreateVertexBuffer(Device, Mesh.Vertices);
		delete Mesh.Faces;
		delete Mesh.Vertices;

		// CreateIndexBuffer(Device, Indices);
		//CreateVertexBuffer(Device, Vertices);
	}

	void Free()
	{
		m_IndexBuffer->Release();
		m_VertexBuffer->Release();
	}

private:
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

struct test_texture
{
	ID3D11ShaderResourceView*	m_ShaderResourceView;
	ID3D11SamplerState*			m_SamplerState; // TODO: not sure where to slap this bad boy down the line

	void Initialize(ID3D11Device* Device, const char* Filepath)
	{
		HRESULT Result = S_OK;

		stbi_set_flip_vertically_on_load(false);
		int32 Width = 0;
		int32 Height = 0;
		int32 N = 0;
		unsigned char* Data = stbi_load(Filepath, &Width, &Height, &N, STBI_rgb_alpha);
		uint32* ColorBuffer = new uint32[(uint32)(Width * Height)];

		for (int32 i = 0; i < Width * Height * 4; i = i + 4)
		{
			ColorBuffer[i / 4] = Data[i] + (Data[i + 1] << 8) + (Data[i + 2] << 16) + (Data[i + 3] << 24);
		}
		stbi_image_free(Data);

		D3D11_SUBRESOURCE_DATA TextureData;
		TextureData.pSysMem = ColorBuffer;
		TextureData.SysMemPitch = Width * 4;
		TextureData.SysMemSlicePitch = 0;

		D3D11_TEXTURE2D_DESC TextureDescription = { 0 };
		TextureDescription.Width = Width;
		TextureDescription.Height = Height;
		TextureDescription.MipLevels = 1;
		TextureDescription.ArraySize = 1;
		TextureDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		TextureDescription.SampleDesc.Count = 1;
		TextureDescription.SampleDesc.Quality = 0;
		TextureDescription.Usage = D3D11_USAGE_DEFAULT;
		TextureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		TextureDescription.CPUAccessFlags = 0;
		TextureDescription.MiscFlags = 0;

		ID3D11Texture2D* Texture = NULL;
		Result = Device->CreateTexture2D(&TextureDescription, &TextureData, &Texture);

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDescription = { };
		SRVDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDescription.Texture2D.MostDetailedMip = 0;
		SRVDescription.Texture2D.MipLevels = -1;
		Result = Device->CreateShaderResourceView(Texture, &SRVDescription, &m_ShaderResourceView);

		D3D11_SAMPLER_DESC SamplerDescription = { };
		SamplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;
		SamplerDescription.MinLOD = 0;
		SamplerDescription.MaxLOD = D3D11_FLOAT32_MAX;
		Result = Device->CreateSamplerState(&SamplerDescription, &m_SamplerState);

		delete[] ColorBuffer;
		Texture->Release();
	}

	void Free()
	{
		m_ShaderResourceView->Release();
		m_SamplerState->Release();
	}
};

struct test_object
{
	vertex_shader			m_VertexShader;
	pixel_shader			m_PixelShader;
	test_mesh*				m_Mesh;
	test_texture*			m_Texture;
	ID3D11RasterizerState*	m_RasterizerState; // TODO: maybe this should be part of renderpasses?
	DirectX::XMFLOAT3		m_Position;

	void Initialize(ID3D11Device* Device, test_mesh* Mesh, test_texture* Texture)
	{
		m_Mesh = Mesh;
		m_Texture = Texture;
		m_VertexShader.Initialize(Device);
		m_PixelShader.Initialize(Device);
		D3D11_RASTERIZER_DESC RasterizerStateDescription = { };
		RasterizerStateDescription.FillMode = D3D11_FILL_WIREFRAME;
		RasterizerStateDescription.CullMode = D3D11_CULL_NONE;
		HRESULT Result = Device->CreateRasterizerState(&RasterizerStateDescription, &m_RasterizerState);
	}

	void SetState(ID3D11DeviceContext* DeviceContext)
	{
		uint32 Stride = sizeof(vertex_3D);
		uint32 Offset = 0;
		DeviceContext->IASetIndexBuffer(m_Mesh->m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		DeviceContext->IASetVertexBuffers(0, 1, &m_Mesh->m_VertexBuffer, &Stride, &Offset);
		DeviceContext->IASetInputLayout(m_VertexShader.InputLayout);
		DeviceContext->VSSetShader(m_VertexShader.Shader, 0, 0);
		DeviceContext->VSSetConstantBuffers(0, 1, &m_VertexShader.ConstantBuffer);
		DeviceContext->PSSetShader(m_PixelShader.Shader, 0, 0);
		DeviceContext->PSSetShaderResources(0, 1, &m_Texture->m_ShaderResourceView);
		DeviceContext->PSSetSamplers(0, 1, &m_Texture->m_SamplerState);
		DeviceContext->OMSetBlendState(0, 0, 0xffffffff);
		//DeviceContext->RSSetState(NULL);
	}

	void Free()
	{
		m_RasterizerState->Release();
		m_VertexShader.Free();
		m_PixelShader.Free();
		m_Mesh->Free();
		m_Texture->Free();
		delete m_Mesh;
		delete m_Texture;
	}
};

struct test_cb_object
{
	DirectX::XMMATRIX WorldTransform;
	DirectX::XMMATRIX MVPTransform;
};

struct test_cb_frame
{
	test_cb_frame()
	{
		Light.Direction = XMFLOAT3(0.25f, 0.5f, -1.0f);
		Light.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
		Light.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	directional_light Light;
};

struct test_object2
{
	character_vertex_shader	m_VertexShader;
	character_pixel_shader	m_PixelShader;
	test_mesh*				m_Mesh;
	test_texture*			m_Texture;
	ID3D11RasterizerState*	m_RasterizerState; // TODO: maybe this should be part of renderpasses?
	DirectX::XMFLOAT3		m_Position;
	test_cb_object			m_CbPerObject;
	test_cb_frame			m_CbPerFrame;

	void Initialize(ID3D11Device* Device, test_mesh* Mesh, test_texture* Texture)
	{
		m_Mesh = Mesh;
		m_Texture = Texture;
		m_VertexShader.Initialize(Device);
		m_PixelShader.Initialize(Device);
		D3D11_RASTERIZER_DESC RasterizerStateDescription = { };
		RasterizerStateDescription.FillMode = D3D11_FILL_WIREFRAME;
		RasterizerStateDescription.CullMode = D3D11_CULL_NONE;
		HRESULT Result = Device->CreateRasterizerState(&RasterizerStateDescription, &m_RasterizerState);
	}

	void SetState(ID3D11DeviceContext* DeviceContext)
	{
		uint32 Stride = sizeof(vertex_3D);
		uint32 Offset = 0;
		DeviceContext->IASetIndexBuffer(m_Mesh->m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		DeviceContext->IASetVertexBuffers(0, 1, &m_Mesh->m_VertexBuffer, &Stride, &Offset);
		DeviceContext->IASetInputLayout(m_VertexShader.InputLayout);
		DeviceContext->VSSetShader(m_VertexShader.Shader, 0, 0);
		DeviceContext->VSSetConstantBuffers(0, 1, &m_VertexShader.ConstantBuffer);
		DeviceContext->PSSetShader(m_PixelShader.Shader, 0, 0);
		DeviceContext->PSSetShaderResources(0, 1, &m_Texture->m_ShaderResourceView);
		DeviceContext->PSSetConstantBuffers(0, 1, &m_PixelShader.ConstantBuffer);
		DeviceContext->PSSetSamplers(0, 1, &m_Texture->m_SamplerState);
		DeviceContext->OMSetBlendState(0, 0, 0xffffffff);

		//DeviceContext->RSSetState(NULL);
	}

	void Free()
	{
		m_RasterizerState->Release();
		m_VertexShader.Free();
		m_PixelShader.Free();
		m_Mesh->Free();
		m_Texture->Free();
		delete m_Mesh;
		delete m_Texture;
	}
};