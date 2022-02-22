#include "render_system.h"
#include "vertex.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "mesh_loader.h"
#include "auto_profiler.h"
#include "test_scene.h"
#include "scene_graph.h"

#define RESOLUTION_WIDTH 1280
#define RESOLUTION_HEIGHT 720

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
	m_FontMedium.InitializeFromFile("data/font/iosevka-ss10-regular.ttf", 0, 22, m_Device);
	m_FontSmall.InitializeFromFile("data/font/iosevka-ss10-regular.ttf", 0, SMALL_TEXT_SIZE, m_Device);
	m_OverlayTransform = DirectX::XMMatrixOrthographicLH(RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 1.0f, 1000.0f);
	m_Rectangle.Initialize(m_Device, 0.0f, 0.0f, 1.0f, 1.0f);
}

render_system::~render_system()
{
	m_SwapChain->Release();
	m_Device->Release();
	m_DeviceContext->Release();
	m_MainRenderTargetView->Release();
	m_DepthStencilView->Release();
}

void render_system::Clear()
{
	const float ClearColor[4] = { 0.45f, 0.45f, 0.45f, 1.0f };
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_DeviceContext->ClearRenderTargetView(m_MainRenderTargetView, ClearColor);
}

void render_system::Render(camera* Camera)
{
	PROFILE();
	// Camera transform
	float FOV = Camera->m_FOV * (3.14f / 180.0f);
	DirectX::XMVECTOR CameraPosition = DirectX::XMVectorSet(Camera->m_Position.x, Camera->m_Position.y, Camera->m_Position.z, 0.0f);
	DirectX::XMMATRIX ViewMatrix = DirectX::XMMatrixLookToLH(CameraPosition, Camera->m_Target, Camera->m_UpDirection);
	DirectX::XMMATRIX ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(FOV, (float)RESOLUTION_WIDTH / (float)RESOLUTION_HEIGHT, 1.0f, 1000.0f);
	DirectX::XMMATRIX VPMatrix = ViewMatrix * ProjectionMatrix;
}

void render_system::Render(scene_graph* Scene)
{
	camera* Camera = Scene->m_Camera;
	float FOV = Camera->m_FOV * (3.14f / 180.0f);
	DirectX::XMVECTOR CameraPosition = DirectX::XMVectorSet(Camera->m_Position.x, Camera->m_Position.y, Camera->m_Position.z, 0.0f);
	DirectX::XMMATRIX ViewMatrix = DirectX::XMMatrixLookToLH(CameraPosition, Camera->m_Target, Camera->m_UpDirection);
	DirectX::XMMATRIX ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(FOV, (float)RESOLUTION_WIDTH / (float)RESOLUTION_HEIGHT, 1.0f, 1000.0f);
	DirectX::XMMATRIX VPMatrix = ViewMatrix * ProjectionMatrix;
	
	m_DeviceContext->IASetInputLayout(m_RenderStateCharacter.m_OffscreenPass.m_InputLayout);
	m_DeviceContext->VSSetShader(m_RenderStateCharacter.m_OffscreenPass.m_VertexShader, 0, 0);
	m_DeviceContext->VSSetConstantBuffers(0, 1, &m_RenderStateCharacter.m_OffscreenPass.m_CbPerObject);
	m_DeviceContext->PSSetShader(m_RenderStateCharacter.m_OffscreenPass.m_PixelShader, 0, 0);
	m_DeviceContext->PSSetConstantBuffers(0, 1, &m_RenderStateCharacter.m_OffscreenPass.m_CbPerFrame);

	const float ClearColor[4] = { 0.45f, 0.45f, 0.45f, 1.0f };
	m_DeviceContext->ClearRenderTargetView(m_RenderStateCharacter.m_OffScreenTarget[0], ClearColor);
	m_DeviceContext->ClearDepthStencilView(m_RenderStateCharacter.m_OffscreenDepthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_DeviceContext->OMSetRenderTargets(1, &m_RenderStateCharacter.m_OffScreenTarget[0], m_RenderStateCharacter.m_OffscreenDepthView);

	// P1
	float OffFOV = 59.0f * (3.14f / 180.0f);
	DirectX::XMVECTOR OffCameraPosition = DirectX::XMVectorSet(0.0f, 0.0f, -9.3f, 0.0f);
	DirectX::XMVECTOR OffCameraTarget = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR OffCameraUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX OffViewMatrix = DirectX::XMMatrixLookToLH(OffCameraPosition, OffCameraTarget, OffCameraUp);
	DirectX::XMMATRIX OffProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(OffFOV, (float)RESOLUTION_WIDTH / (float)RESOLUTION_HEIGHT, 1.0f, 1000.0f);
	DirectX::XMMATRIX OffVPMatrix = OffViewMatrix * OffProjectionMatrix;
	SceneGraph->m_CharacterNodes[0].m_OffscreenTransforms.m_MVPTransform = OffVPMatrix;
	Scene->m_CharacterNodes[0].m_OffscreenTransforms.m_MVPTransform = DirectX::XMMatrixTranspose(OffVPMatrix) * Scene->m_CharacterNodes[0].m_OffscreenTransforms.m_WorldTransform;
	CharacterOffscreenPass(&Scene->m_CharacterNodes[0], m_RenderStateCharacter.m_OffScreenTarget[0]);

	m_DeviceContext->UpdateSubresource(
		m_RenderStateCharacter.m_OutlinePass.m_CbPerFrame, 0, NULL,
		&Scene->m_CharacterNodes[0].m_OutlineProperties, 0, 0);

	uint32 Stride = sizeof(vertex_3D);
	uint32 Offset = 0;
	m_DeviceContext->RSSetState(m_RenderStateCharacter.m_OutlineRasterizer);
	m_DeviceContext->VSSetShader(m_RenderStateCharacter.m_OutlinePass.m_VertexShader, NULL, 0);
	m_DeviceContext->PSSetShader(m_RenderStateCharacter.m_OutlinePass.m_PixelShader, NULL, 0);
	m_DeviceContext->PSSetConstantBuffers(0, 1, &m_RenderStateCharacter.m_OutlinePass.m_CbPerFrame);
	m_DeviceContext->DrawIndexed(Scene->m_CharacterNodes[0].m_Mesh.m_IndexEntryCount, 0, 0);
	m_DeviceContext->RSSetState(NULL);

	// TODO: Offscreen target

	m_DeviceContext->OMSetRenderTargets(1, &m_MainRenderTargetView, m_DepthStencilView);

	// Sprite
	m_DeviceContext->IASetVertexBuffers(0, 1, &m_TexturedQuad.m_VertexBuffer, &Stride, &Offset);
	m_DeviceContext->VSSetShader(m_TexturedQuad.m_VertexShader, NULL, 0);
	m_DeviceContext->PSSetShader(m_TexturedQuad.m_PixelShader, NULL, 0);
	XMMATRIX Matrices[2] = { XMMatrixScaling(4.0f, 4.0f, 1.0f) };
	Matrices[1] = XMMatrixTranspose(Matrices[0] * VPMatrix);
	m_DeviceContext->UpdateSubresource(
		m_RenderStateCharacter.m_OffscreenPass.m_CbPerObject, 0, NULL,
		&Matrices, 0, 0);
	m_DeviceContext->PSSetSamplers(0, 1, &m_RenderStateCharacter.m_SpriteSampler);
	m_DeviceContext->PSSetShaderResources(0, 1, &m_RenderStateCharacter.m_SpriteTexture[0]);
	m_DeviceContext->Draw(6, 0);
}

void render_system::Present()
{
	m_SwapChain->Present(0, 0);
}

void render_system::DrawString(float XOffset, float YOffset, float Scale, const char* String ...)
{
	char Buffer[512];
	va_list args;
	va_start(args, String);
	vsnprintf(Buffer, 512, String, args);
	va_end(args);

	RenderText(&m_FontMedium, XOffset, YOffset, Scale, Buffer);
}

void render_system::DrawStringSmall(float XOffset, float YOffset, float Scale, const char* String ...)
{
	char Buffer[512];
	va_list args;
	va_start(args, String);
	vsnprintf(Buffer, 512, String, args);
	va_end(args);

	RenderText(&m_FontSmall, XOffset, YOffset, Scale, Buffer);
}

void render_system::DrawStringSmallRightAligned(float XOffset, float YOffset, float Scale, const char* String ...)
{
	char Buffer[512];
	va_list args;
	va_start(args, String);
	vsnprintf(Buffer, 512, String, args);
	va_end(args);

	RenderTextRightAligned(&m_FontSmall, XOffset, YOffset, Scale, Buffer);
}

void render_system::DrawRectangle(XMMATRIX* Transform, XMFLOAT4 Color)
{
	m_DeviceContext->OMSetRenderTargets(1, &m_MainRenderTargetView, NULL);
	m_Rectangle.SetTransform(Transform);
	m_Rectangle.Render(m_DeviceContext, Color, &m_OverlayTransform);
	m_DeviceContext->OMSetRenderTargets(1, &m_MainRenderTargetView, m_DepthStencilView);
}

void render_system::InitScene()
{
	vertex_3D Vertices[] =
	{
		vertex_3D(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f),
		vertex_3D(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, -1.0f),
		vertex_3D(1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f),
		vertex_3D(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f),
		vertex_3D(1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f),
		vertex_3D(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f)
	};

	D3D11_BUFFER_DESC VertexBufferDescription = { 0 };
	VertexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDescription.ByteWidth = sizeof(vertex_3D) * ARRAYSIZE(Vertices);
	VertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDescription.CPUAccessFlags = 0;
	VertexBufferDescription.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA VertexBufferData = { 0 };
	VertexBufferData.pSysMem = Vertices;
	HRESULT Result = m_Device->CreateBuffer(&VertexBufferDescription, &VertexBufferData, &m_TexturedQuad.m_VertexBuffer);

	ID3D10Blob* VSBuffer = NULL;
	Result = D3DCompileFromFile(L"Shaders/TexturedQuad.fx", 0, 0, "VS", "vs_4_0", 0, 0, &VSBuffer, 0);
	Result = m_Device->CreateVertexShader(VSBuffer->GetBufferPointer(), VSBuffer->GetBufferSize(), NULL, &m_TexturedQuad.m_VertexShader);
	VSBuffer->Release();

	ID3D10Blob* PSBuffer = NULL;
	Result = D3DCompileFromFile(L"Shaders/TexturedQuad.fx", 0, 0, "PS", "ps_4_0", 0, 0, &PSBuffer, 0);
	Result = m_Device->CreatePixelShader(PSBuffer->GetBufferPointer(), PSBuffer->GetBufferSize(), NULL, &m_TexturedQuad.m_PixelShader);
	PSBuffer->Release();

	mesh Mesh = assets::LoadMeshFromFile("data/shitman.dae");
	uint32 IndexEntryCount = Mesh.FaceCount * 3;
	uint32 VertexCount = Mesh.VertexCount;

	D3D11_BUFFER_DESC IndexBufferDescription = { 0 };
	IndexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDescription.ByteWidth = sizeof(uint32) * IndexEntryCount;
	IndexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDescription.CPUAccessFlags = 0;
	IndexBufferDescription.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA IndexBufferData = { 0 };
	IndexBufferData.pSysMem = Mesh.Faces;
	Result = m_Device->CreateBuffer(&IndexBufferDescription, &IndexBufferData, &SceneGraph->m_CharacterNodes[0].m_Mesh.m_IndexBuffer);
	SceneGraph->m_CharacterNodes[0].m_Mesh.m_IndexEntryCount = IndexEntryCount;
	SceneGraph->m_CharacterNodes[0].m_Mesh.m_VertexCount = Mesh.VertexCount;
	SceneGraph->m_CharacterNodes[0].m_Mesh.CreateVertexBuffer(m_Device, Mesh.Vertices);
	delete Mesh.Faces;
	delete Mesh.Vertices;

	SceneGraph->m_CharacterNodes[0].m_OffscreenLights.m_DirectionalLight.Ambient = DirectX::XMFLOAT4(0.35f, 0.35f, 0.35f, 1.0f);
	SceneGraph->m_CharacterNodes[0].m_OffscreenLights.m_DirectionalLight.Diffuse = DirectX::XMFLOAT4(0.78f, 0.78f, 0.78f, 1.0f);
	SceneGraph->m_CharacterNodes[0].m_OffscreenLights.m_DirectionalLight.Direction = DirectX::XMFLOAT3(-0.40f, -0.1f, 0.15f);
	DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&SceneGraph->m_CharacterNodes[0].m_OffscreenLights.m_DirectionalLight.Direction);
	Direction = XMVector3Normalize(-Direction);
	XMStoreFloat3(&SceneGraph->m_CharacterNodes[0].m_OffscreenLights.m_DirectionalLight.Direction, Direction);
	XMFLOAT3 Axis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMVECTOR AxisVector = XMLoadFloat3(&Axis);
	XMMATRIX Rotation = XMMatrixRotationAxis(AxisVector, -45.0f * (3.14f / 180.0f));
	SceneGraph->m_CharacterNodes[0].m_OffscreenTransforms.m_WorldTransform = XMMatrixTranspose( DirectX::XMMatrixIdentity() );

	float FOV = 59.0f * (3.14f / 180.0f);
	DirectX::XMVECTOR CameraPosition = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
	DirectX::XMVECTOR CameraTarget = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR CameraUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX ViewMatrix = DirectX::XMMatrixLookToLH(CameraPosition, CameraTarget, CameraUp);
	DirectX::XMMATRIX ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(FOV, (float)RESOLUTION_WIDTH / (float)RESOLUTION_HEIGHT, 1.0f, 1000.0f);
	DirectX::XMMATRIX VPMatrix = ViewMatrix * ProjectionMatrix;
	SceneGraph->m_CharacterNodes[0].m_OffscreenTransforms.m_MVPTransform = VPMatrix;

	SceneGraph->m_CharacterNodes[0].m_OutlineProperties.m_Color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	SceneGraph->m_CharacterNodes[0].m_OutlineProperties.m_Thickness = 0.3f;

	InitRenderStates();
}

void render_system::InitRenderStates()
{
	// *** OFFSCREEN CHARACTER PASS
	// Vertex shader
	// TODO: Error checking
	HRESULT Result;
	ID3D10Blob* ErrorBlob = NULL;
	ID3D10Blob* VSBuffer = NULL;
	D3D11_INPUT_ELEMENT_DESC LayoutDescription[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	uint32 InputCount = ARRAYSIZE(LayoutDescription);
	Result = D3DCompileFromFile(L"Shaders/CharacterOffscreen.fx", 0, 0, "VS", "vs_4_0", D3DCOMPILE_DEBUG, 0, &VSBuffer, &ErrorBlob);
	if (Result)
	{
		if (ErrorBlob)
		{
			char* Error = (char*)ErrorBlob->GetBufferPointer();
			int b = 2;
		}
	}
	Result = m_Device->CreateVertexShader(
		VSBuffer->GetBufferPointer(), 
		VSBuffer->GetBufferSize(), 
		NULL, 
		&m_RenderStateCharacter.m_OffscreenPass.m_VertexShader);
	Result = m_Device->CreateInputLayout(
		LayoutDescription, InputCount, 
		VSBuffer->GetBufferPointer(), 
		VSBuffer->GetBufferSize(), 
		&m_RenderStateCharacter.m_OffscreenPass.m_InputLayout);
	VSBuffer->Release();

	D3D11_BUFFER_DESC ConstantBufferDescription = { 0 };
	ConstantBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	ConstantBufferDescription.ByteWidth = sizeof(character_node::offscreen_transforms);
	ConstantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDescription.CPUAccessFlags = 0;
	ConstantBufferDescription.MiscFlags = 0;
	m_Device->CreateBuffer(&ConstantBufferDescription, NULL, &m_RenderStateCharacter.m_OffscreenPass.m_CbPerObject);

	// Pixel shader
	ID3D10Blob* PSBuffer;
	Result = D3DCompileFromFile(L"Shaders/CharacterOffscreen.fx", 0, 0, "PS", "ps_4_0", 0, 0, &PSBuffer, 0);
	Result = m_Device->CreatePixelShader(
		PSBuffer->GetBufferPointer(), 
		PSBuffer->GetBufferSize(), 
		NULL, 
		&m_RenderStateCharacter.m_OffscreenPass.m_PixelShader);
	PSBuffer->Release();

	ZeroMemory(&ConstantBufferDescription, sizeof(ConstantBufferDescription));
	ConstantBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	ConstantBufferDescription.ByteWidth = sizeof(character_node::character_lights);
	ConstantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDescription.CPUAccessFlags = 0;
	ConstantBufferDescription.MiscFlags = 0;
	m_Device->CreateBuffer(&ConstantBufferDescription, NULL, &m_RenderStateCharacter.m_OffscreenPass.m_CbPerFrame);
	
	float OFFSCREEN_TEXTURE_WIDTH = 1024;
	float OFFSCREEN_TEXTURE_HEIGHT = 1024;

	// Render target
	ID3D11Texture2D* OffscreenTexture = NULL;
	D3D11_TEXTURE2D_DESC TextureDescription = { 0 };
	TextureDescription.Width = OFFSCREEN_TEXTURE_WIDTH;
	TextureDescription.Height = OFFSCREEN_TEXTURE_HEIGHT;
	TextureDescription.MipLevels = 1;
	TextureDescription.ArraySize = 1;
	TextureDescription.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	TextureDescription.SampleDesc.Count = 1;
	TextureDescription.Usage = D3D11_USAGE_DEFAULT;
	TextureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	TextureDescription.CPUAccessFlags = 0;
	TextureDescription.MiscFlags = 0;
	Result = m_Device->CreateTexture2D(&TextureDescription, NULL, &OffscreenTexture);
	if (Result)
		int b = 2;

	D3D11_RENDER_TARGET_VIEW_DESC RenderTargetDescription;
	RenderTargetDescription.Format = TextureDescription.Format;
	RenderTargetDescription.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RenderTargetDescription.Texture2D.MipSlice = 0;
	m_Device->CreateRenderTargetView(OffscreenTexture, &RenderTargetDescription, &m_RenderStateCharacter.m_OffScreenTarget[0]);

	// Shader resource
	D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceDescription;
	ShaderResourceDescription.Format = TextureDescription.Format;
	ShaderResourceDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderResourceDescription.Texture2D.MostDetailedMip = 0;
	ShaderResourceDescription.Texture2D.MipLevels = 1;
	m_Device->CreateShaderResourceView(OffscreenTexture, &ShaderResourceDescription, &m_RenderStateCharacter.m_SpriteTexture[0]);
	OffscreenTexture->Release();

	ID3D11Texture2D* DepthStencilBuffer = NULL;
	D3D11_TEXTURE2D_DESC DepthStencilDescription = { 0 };
	DepthStencilDescription.Width = OFFSCREEN_TEXTURE_WIDTH;
	DepthStencilDescription.Height = OFFSCREEN_TEXTURE_HEIGHT;
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
	Result = m_Device->CreateDepthStencilView(DepthStencilBuffer, NULL, &m_RenderStateCharacter.m_OffscreenDepthView);
	DepthStencilBuffer->Release();

	// *** OUTLINE PASS
	m_RenderStateCharacter.m_OutlinePass.m_InputLayout = m_RenderStateCharacter.m_OffscreenPass.m_InputLayout;
	Result = D3DCompileFromFile(L"Shaders/CharacterOutline.fx", 0, 0, "VS", "vs_4_0", D3DCOMPILE_DEBUG, 0, &VSBuffer, &ErrorBlob);
	if (Result)
	{
		if (ErrorBlob)
		{
			char* Error = (char*)ErrorBlob->GetBufferPointer();
			int b = 2;
		}
	}
	Result = m_Device->CreateVertexShader(
		VSBuffer->GetBufferPointer(),
		VSBuffer->GetBufferSize(),
		NULL,
		&m_RenderStateCharacter.m_OutlinePass.m_VertexShader);
	VSBuffer->Release();
	
	ZeroMemory(&ConstantBufferDescription, sizeof(ConstantBufferDescription));
	ConstantBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	ConstantBufferDescription.ByteWidth = sizeof(character_node::outline_properties);
	ConstantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDescription.CPUAccessFlags = 0;
	ConstantBufferDescription.MiscFlags = 0;
	m_Device->CreateBuffer(&ConstantBufferDescription, NULL, &m_RenderStateCharacter.m_OutlinePass.m_CbPerFrame);

	D3D11_RASTERIZER_DESC RasterizerStateDescription;
	ZeroMemory(&RasterizerStateDescription, sizeof(RasterizerStateDescription));
	RasterizerStateDescription.FillMode = D3D11_FILL_SOLID;
	RasterizerStateDescription.CullMode = D3D11_CULL_FRONT;
	RasterizerStateDescription.FrontCounterClockwise = false;
	m_Device->CreateRasterizerState(&RasterizerStateDescription, &m_RenderStateCharacter.m_OutlineRasterizer);

	Result = D3DCompileFromFile(L"Shaders/CharacterOutline.fx", 0, 0, "PS", "ps_4_0", 0, 0, &PSBuffer, 0);
	Result = m_Device->CreatePixelShader(
		PSBuffer->GetBufferPointer(),
		PSBuffer->GetBufferSize(),
		NULL,
		&m_RenderStateCharacter.m_OutlinePass.m_PixelShader);
	PSBuffer->Release();

	// *** SPRITE PASS
	D3D11_SAMPLER_DESC SamplerDescription = { };
	SamplerDescription.Filter = D3D11_FILTER_ANISOTROPIC;
	SamplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDescription.MinLOD = 0;
	SamplerDescription.MaxLOD = D3D11_FLOAT32_MAX;
	Result = m_Device->CreateSamplerState(&SamplerDescription, &m_RenderStateCharacter.m_SpriteSampler);
}

void render_system::CharacterOffscreenPass(character_node* Node, ID3D11RenderTargetView* RenderTarget)
{
	// TODO: Maps
	uint32 Stride = sizeof(vertex_3D);
	uint32 Offset = 0;
	m_DeviceContext->IASetIndexBuffer(Node->m_Mesh.m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	m_DeviceContext->IASetVertexBuffers(0, 1, &Node->m_Mesh.m_VertexBuffer, &Stride, &Offset);
	// TODO: Switch to dynamic usage (map/unmap)
	m_DeviceContext->UpdateSubresource(
		m_RenderStateCharacter.m_OffscreenPass.m_CbPerObject, 0, NULL, 
		&Node->m_OffscreenTransforms, 0, 0);
	m_DeviceContext->UpdateSubresource(
		m_RenderStateCharacter.m_OffscreenPass.m_CbPerFrame, 0, NULL,
		&Node->m_OffscreenLights, 0, 0);
	// TODO: Offscreen target
	m_DeviceContext->DrawIndexed(Node->m_Mesh.m_IndexEntryCount, 0, 0);
}

void render_system::RenderTest(test_object2* TestObject, DirectX::XMMATRIX VPMatrix)
{
// Object transform
	DirectX::XMMATRIX TranslationMatrix = DirectX::XMMatrixTranslation(TestObject->m_Position.x, TestObject->m_Position.y, TestObject->m_Position.z);
	//DirectX::XMMATRIX RotationMatrix = DirectX::XMMatrixRotationY(Rotation);
	DirectX::XMMATRIX RotationMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX ScalingMatrix = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
	TestObject->m_CbPerObject.WorldTransform = ScalingMatrix * RotationMatrix * TranslationMatrix;
	TestObject->m_CbPerObject.MVPTransform = DirectX::XMMatrixTranspose(TestObject->m_CbPerObject.WorldTransform * VPMatrix);

	// Update constant buffer
	TestObject->SetState(m_DeviceContext);
	m_DeviceContext->UpdateSubresource(TestObject->m_VertexShader.ConstantBuffer, 0, NULL, &TestObject->m_CbPerObject, 0, 0);
	m_DeviceContext->UpdateSubresource(TestObject->m_PixelShader.ConstantBuffer, 0, NULL, &TestObject->m_CbPerFrame, 0, 0);
	m_DeviceContext->DrawIndexed(TestObject->m_Mesh->m_IndexEntryCount, 0, 0);
}

void render_system::RenderText(d3d11_font* Font, float XOffset, float YOffset, float Scale, const char* String)
{
	PROFILE();
	// TODO: transform a quad instead of reuploading positions for every character
	// TODO: move all the d3d state so it's set only once before rendering anything 2d 
	int32 StringLength = strlen(String);
	float BlendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 Color(1.0f, 1.0f, 1.0f, 1.0f);
	uint32 Stride[1] = { sizeof(float) * 4 };
	uint32 Offset[1] = { 0 };
	
	m_DeviceContext->IASetInputLayout(Font->m_InputLayout);
	m_DeviceContext->IASetVertexBuffers(0, 1, &Font->m_VertexBuffer, Stride, Offset);
	m_DeviceContext->VSSetShader(Font->m_VertexShader, 0, 0);
	m_DeviceContext->VSSetConstantBuffers(0, 1, &Font->m_ProjectionBuffer);
	m_DeviceContext->PSSetConstantBuffers(0, 1, &Font->m_ColorBuffer);
	m_DeviceContext->UpdateSubresource(Font->m_ColorBuffer, 0, NULL, &Color, 0, 0);
	m_DeviceContext->PSSetShader(Font->m_PixelShader, 0, 0);
	m_DeviceContext->PSSetSamplers(0, 1, &Font->m_SamplerState);
	m_DeviceContext->OMSetBlendState(Font->m_BlendState, BlendFactor, 0xFFFFFFFF);
	m_DeviceContext->OMSetRenderTargets(1, &m_MainRenderTargetView, NULL);

	//XOffset = (XOffset * (RESOLUTION_WIDTH / 2.0f));
	//YOffset = (YOffset * (RESOLUTION_HEIGHT / 2.0f));
	//XOffset -= (RESOLUTION_WIDTH / 2.0f);
	//YOffset -= (RESOLUTION_HEIGHT / 2.0f);
	float LineStartX = XOffset;

	for (int32 i = 0; i < StringLength; i++)
	{
		const char C = String[i];
		if (C == '\n')
		{
			XOffset = LineStartX;
			YOffset -= (float)Font->m_PixelHeight;
			continue;
		}
		d3d11_character* Character = &Font->m_Characters[String[i]];
		float X = XOffset + (float)Character->m_BearingX * Scale;
		float Y = YOffset - (float)(Character->m_Height - Character->m_BearingY) * Scale;
		float Width = (float)Character->m_Width * Scale;
		float Height = (float)Character->m_Height * Scale;

		DirectX::XMMATRIX Transform = DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(Width, Height, 1.0f) * DirectX::XMMatrixTranslation(X, Y, 2.0f) * m_OverlayTransform);
		m_DeviceContext->UpdateSubresource(Font->m_ProjectionBuffer, 0, NULL, &Transform, 0, 0);
		
		m_DeviceContext->PSSetShaderResources(0, 1, &Character->m_ShaderResourceView);
		m_DeviceContext->Draw(6, 0);
		XOffset += (Character->m_Advance >> 6) * Scale;
	}
	m_DeviceContext->OMSetRenderTargets(1, &m_MainRenderTargetView, m_DepthStencilView);
}

void render_system::RenderTextRightAligned(d3d11_font* Font, float XOffset, float YOffset, float Scale, const char* String)
{
	PROFILE();
	// TODO: transform a quad instead of reuploading positions for every character
	int32 StringLength = strlen(String);
	float BlendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 Color(1.0f, 1.0f, 1.0f, 1.0f);
	uint32 Stride[1] = { sizeof(float) * 4 };
	uint32 Offset[1] = { 0 };

	m_DeviceContext->IASetInputLayout(Font->m_InputLayout);
	m_DeviceContext->IASetVertexBuffers(0, 1, &Font->m_VertexBuffer, Stride, Offset);
	m_DeviceContext->VSSetShader(Font->m_VertexShader, 0, 0);
	m_DeviceContext->VSSetConstantBuffers(0, 1, &Font->m_ProjectionBuffer);
	m_DeviceContext->PSSetConstantBuffers(0, 1, &Font->m_ColorBuffer);
	m_DeviceContext->UpdateSubresource(Font->m_ColorBuffer, 0, NULL, &Color, 0, 0);
	m_DeviceContext->PSSetShader(Font->m_PixelShader, 0, 0);
	m_DeviceContext->PSSetSamplers(0, 1, &Font->m_SamplerState);
	m_DeviceContext->OMSetBlendState(Font->m_BlendState, BlendFactor, 0xFFFFFFFF);
	m_DeviceContext->OMSetRenderTargets(1, &m_MainRenderTargetView, NULL);

	//XOffset = (XOffset * (RESOLUTION_WIDTH / 2.0f));
	//YOffset = (YOffset * (RESOLUTION_HEIGHT / 2.0f));
	//XOffset -= (RESOLUTION_WIDTH / 2.0f);
	//YOffset -= (RESOLUTION_HEIGHT / 2.0f);
	float LineStartX = XOffset;

	for (int32 i = StringLength - 1; i >= 0; i--)
	{
		const char C = String[i];
		if (C == '\n')
		{
			XOffset = LineStartX;
			YOffset -= (float)Font->m_PixelHeight;
			continue;
		}
		d3d11_character* Character = &Font->m_Characters[String[i]];
		XOffset -= (Character->m_Advance >> 6) * Scale;
		float X = XOffset + (float)Character->m_BearingX * Scale;
		float Y = YOffset - (float)(Character->m_Height - Character->m_BearingY) * Scale;
		float Width = (float)Character->m_Width * Scale;
		float Height = (float)Character->m_Height * Scale;

		
		DirectX::XMMATRIX Transform = DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(Width, Height, 1.0f) * DirectX::XMMatrixTranslation(X, Y, 2.0f) * m_OverlayTransform);
		m_DeviceContext->UpdateSubresource(Font->m_ProjectionBuffer, 0, NULL, &Transform, 0, 0);
		m_DeviceContext->PSSetShaderResources(0, 1, &Character->m_ShaderResourceView);
		m_DeviceContext->Draw(6, 0);
	}

	m_DeviceContext->OMSetRenderTargets(1, &m_MainRenderTargetView, m_DepthStencilView);
}
