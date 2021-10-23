#include "render_system.h"
#include "vertex.h"

#define RESOLUTION_WIDTH 1280
#define RESOLUTION_HEIGHT 720

struct test_mesh
{
	ID3D11Buffer*	m_IndexBuffer;
	ID3D11Buffer*	m_VertexBuffer;
	uint32			m_IndexEntryCount;
	uint32			m_VertexCount;
	void Initialize(ID3D11Device* Device, uint32* Indices, uint32 IndexEntryCount, vertex* Vertices, uint32 VertexCount)
	{
		m_IndexEntryCount = IndexEntryCount;
		m_VertexCount = VertexCount;
		CreateIndexBuffer(Device, Indices);
		CreateVertexBuffer(Device, Vertices);
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

	void CreateVertexBuffer(ID3D11Device* Device, vertex* Vertices)
	{
		D3D11_BUFFER_DESC VertexBufferDescription = { 0 };
		VertexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		VertexBufferDescription.ByteWidth = sizeof(vertex) * m_VertexCount;
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
	
	void Initialize(ID3D11Device* Device, uint32 ColorMask)
	{
		HRESULT Result = S_OK;
		uint32 Width = 256;
		uint32 Height = 256;
		uint32* ColorBuffer = new uint32[Width * Height];

		for (uint32 i = 0; i < Width * Height; i++)
		{
			ColorBuffer[i] = ColorMask;
		}

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

		delete ColorBuffer;
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
		uint32 Stride = sizeof(vertex);
		uint32 Offset = 0;
		DeviceContext->IASetIndexBuffer(m_Mesh->m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		DeviceContext->IASetVertexBuffers(0, 1, &m_Mesh->m_VertexBuffer, &Stride, &Offset);
		DeviceContext->IASetInputLayout(m_VertexShader.InputLayout);
		DeviceContext->VSSetShader(m_VertexShader.Shader, 0, 0);
		DeviceContext->VSSetConstantBuffers(0, 1, &m_VertexShader.ConstantBuffer);
		DeviceContext->PSSetShader(m_PixelShader.Shader, 0, 0);
		DeviceContext->PSSetShaderResources(0, 1, &m_Texture->m_ShaderResourceView);
		DeviceContext->PSSetSamplers(0, 1, &m_Texture->m_SamplerState);
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

test_object TestObject;
test_object TestObject2;

render_system::render_system(HWND Window, HINSTANCE Instance)
{
	// TODO: Get settings from file
	DXGI_MODE_DESC BufferDescription = { 0 };
	BufferDescription.Width = RESOLUTION_WIDTH;
	BufferDescription.Height = RESOLUTION_HEIGHT;
	BufferDescription.RefreshRate.Numerator = 60;
	BufferDescription.RefreshRate.Denominator = 1;
	BufferDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	BufferDescription.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	BufferDescription.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SWAP_CHAIN_DESC SwapChainDescription = { 0 };
	SwapChainDescription.BufferDesc = BufferDescription;
	SwapChainDescription.SampleDesc.Count = 1;
	SwapChainDescription.SampleDesc.Quality = 0;
	SwapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDescription.BufferCount = 1;
	SwapChainDescription.OutputWindow = Window;
	SwapChainDescription.Windowed = true;
	SwapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// TODO: Error checking
	// TODO: Seperate the device and swapchain creation 
	HRESULT Result = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&SwapChainDescription,
		&m_SwapChain,
		&m_Device,
		NULL,
		&m_DeviceContext);
	ID3D11Texture2D* BackBuffer = NULL;
	Result = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);
	Result = m_Device->CreateRenderTargetView(BackBuffer, NULL, &m_MainRenderTargetView);
	BackBuffer->Release();

	// Depth/stencil buffer
	ID3D11Texture2D* DepthStencilBuffer = NULL;
	D3D11_TEXTURE2D_DESC DepthStencilDescription = { 0 };
	DepthStencilDescription.Width = RESOLUTION_WIDTH;
	DepthStencilDescription.Height = RESOLUTION_WIDTH;
	DepthStencilDescription.MipLevels = 1;
	DepthStencilDescription.ArraySize = 1;
	DepthStencilDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilDescription.SampleDesc.Count = 1;
	DepthStencilDescription.SampleDesc.Quality = 0;
	DepthStencilDescription.Usage = D3D11_USAGE_DEFAULT;
	DepthStencilDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthStencilDescription.CPUAccessFlags = 0;
	DepthStencilDescription.MiscFlags = 0;
	Result = m_Device->CreateTexture2D(&DepthStencilDescription, NULL, &DepthStencilBuffer);
	Result = m_Device->CreateDepthStencilView(DepthStencilBuffer, NULL, &m_DepthStencilView);
	DepthStencilBuffer->Release();
	m_DeviceContext->OMSetRenderTargets(1, &m_MainRenderTargetView, m_DepthStencilView);

	// Viewport
	D3D11_VIEWPORT Viewport = { 0 };
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;
	// TODO: something with resolution
	Viewport.Width = RESOLUTION_WIDTH;
	Viewport.Height = RESOLUTION_HEIGHT;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	m_DeviceContext->RSSetViewports(1, &Viewport);

	// ??
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// TEST 
	InitScene();
}

render_system::~render_system()
{
	m_SwapChain->Release();
	m_Device->Release();
	m_DeviceContext->Release();
	m_MainRenderTargetView->Release();
	m_DepthStencilView->Release();
}

void render_system::Render()
{
	const float ClearColor[4] = { 0.15f, 0.15f, 0.15f, 1.0f };
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_DeviceContext->ClearRenderTargetView(m_MainRenderTargetView, ClearColor);

	// Camera transform
	DirectX::XMVECTOR CameraPosition = DirectX::XMVectorSet(-2.0f, 1.75f, -3.0f, 0.0f);
	DirectX::XMVECTOR CameraTarget = DirectX::XMVectorSet(0.0f, 0.0f, 4.0f, 0.0f);
	DirectX::XMVECTOR CameraUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	float FOV = 59.0f * (3.14f / 180.0f);
	DirectX::XMMATRIX ViewMatrix = DirectX::XMMatrixLookAtLH(CameraPosition, CameraTarget, CameraUp);
	DirectX::XMMATRIX ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(FOV, (float)RESOLUTION_WIDTH / (float)RESOLUTION_HEIGHT, 1.0f, 1000.0f);
	DirectX::XMMATRIX VPMatrix = ViewMatrix * ProjectionMatrix;

	RenderTest(&TestObject, VPMatrix);
	RenderTest(&TestObject2, VPMatrix);

	m_SwapChain->Present(0, 0);
}

void render_system::InitScene()
{
	uint32 Indices[] =
	{
		// Front Face
		0,  1,  2,
		0,  2,  3,

		// Back Face
		4,  5,  6,
		4,  6,  7,

		// Top Face
		8,  9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23
	};
	vertex Vertices[] =
	{
		// Front Face
		vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

		// Back Face
		vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f),
		vertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
		vertex(1.0f,  1.0f, 1.0f, 0.0f, 0.0f),
		vertex(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f),

		// Top Face
		vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f),
		vertex(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f),
		vertex(1.0f, 1.0f,  1.0f, 1.0f, 0.0f),
		vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f),

		// Bottom Face
		vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
		vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		vertex(1.0f, -1.0f,  1.0f, 0.0f, 0.0f),
		vertex(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f),

		// Left Face
		vertex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f),
		vertex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f),
		vertex(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

		// Right Face
		vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		vertex(1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		vertex(1.0f,  1.0f,  1.0f, 1.0f, 0.0f),
		vertex(1.0f, -1.0f,  1.0f, 1.0f, 1.0f)
	};
	uint32 VertexCount = ARRAYSIZE(Vertices);
	uint32 IndexEntryCount = ARRAYSIZE(Indices);
	test_mesh* Mesh = new test_mesh();
	Mesh->Initialize(m_Device, Indices, IndexEntryCount, Vertices, VertexCount);
	test_texture* Texture = new test_texture();
	Texture->Initialize(m_Device, 0xFF00FFFF);
	TestObject.Initialize(m_Device, Mesh, Texture);
	TestObject.m_Position.x = 0.0f;
	TestObject.m_Position.y = 0.0f;
	TestObject.m_Position.z = 4.0f;

	test_texture* Texture2 = new test_texture();
	Texture2->Initialize(m_Device, 0xFFFF00FF);
	TestObject2.Initialize(m_Device, Mesh, Texture2);
	TestObject2.m_Position.x = -3.0f;
	TestObject2.m_Position.y = 0.0f;
	TestObject2.m_Position.z = 8.0f;
}

void render_system::RenderTest(test_object* TestObject, DirectX::XMMATRIX VPMatrix)
{
	static float Rotation = 0.0f;
	Rotation += 0.0055f;
	if (Rotation > 6.28f)
		Rotation = 0.0f;

	// Object transform
	DirectX::XMMATRIX TranslationMatrix = DirectX::XMMatrixTranslation(TestObject->m_Position.x, TestObject->m_Position.y, TestObject->m_Position.z);
	DirectX::XMMATRIX RotationMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX ScalingMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX ModelMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;
	DirectX::XMMATRIX MVPMatrix = DirectX::XMMatrixTranspose(ModelMatrix * VPMatrix);

	// Update constant buffer
	m_DeviceContext->UpdateSubresource(TestObject->m_VertexShader.ConstantBuffer, 0, NULL, &MVPMatrix, 0, 0);
	TestObject->SetState(m_DeviceContext);

	m_DeviceContext->DrawIndexed(TestObject->m_Mesh->m_IndexEntryCount, 0, 0);
}
