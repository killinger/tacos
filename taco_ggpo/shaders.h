#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "defs.h"
#pragma comment(lib,"d3dcompiler.lib")

struct vertex_shader
{
	ID3D11VertexShader* Shader;
	ID3D11InputLayout*	InputLayout;
	ID3D11Buffer*		ConstantBuffer;
	void Initialize(ID3D11Device* Device)
	{
		// TODO: Error checking
		HRESULT Result;
		ID3D10Blob* VSBuffer = NULL;
		D3D11_INPUT_ELEMENT_DESC LayoutDescription[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		uint32 InputCount = ARRAYSIZE(LayoutDescription);
		Result = D3DCompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_4_0", 0, 0, &VSBuffer, 0);
		Result = Device->CreateVertexShader(VSBuffer->GetBufferPointer(), VSBuffer->GetBufferSize(), NULL, &Shader);
		Result = Device->CreateInputLayout(LayoutDescription, InputCount, VSBuffer->GetBufferPointer(), VSBuffer->GetBufferSize(), &InputLayout);
		VSBuffer->Release();

		D3D11_BUFFER_DESC ConstantBufferDescription = { 0 };
		ConstantBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		ConstantBufferDescription.ByteWidth = sizeof(DirectX::XMMATRIX);
		ConstantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ConstantBufferDescription.CPUAccessFlags = 0;
		ConstantBufferDescription.MiscFlags = 0;
		Device->CreateBuffer(&ConstantBufferDescription, NULL, &ConstantBuffer);
	}
	void Free()
	{
		Shader->Release();
		InputLayout->Release();
		ConstantBuffer->Release();
	}
};

struct pixel_shader
{
	ID3D11PixelShader* Shader;
	void Initialize(ID3D11Device* Device)
	{
		// TODO: Error checking
		HRESULT Result;
		ID3D10Blob* PSBuffer;
		Result = D3DCompileFromFile(L"Effects.fx", 0, 0, "PS", "ps_4_0", 0, 0, &PSBuffer, 0);
		Result = Device->CreatePixelShader(PSBuffer->GetBufferPointer(), PSBuffer->GetBufferSize(), NULL, &Shader);
		PSBuffer->Release();
	}
	void Free()
	{
		Shader->Release();
	}
};