#pragma once

using namespace DirectX;

struct d3d11_rectangle
{
	ID3D11Buffer*		m_VertexBuffer; 
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader*	m_PixelShader;
	ID3D11InputLayout*	m_InputLayout;
	ID3D11Buffer*		m_TransformBuffer;
	ID3D11Buffer*		m_ColorBuffer;
	ID3D11BlendState*	m_BlendState;
	XMMATRIX			m_Transform;

	void Initialize(ID3D11Device* Device, float X, float Y, float Width, float Height)
	{
		HRESULT Result;
		ID3D10Blob* VSBuffer = NULL;
		D3D11_INPUT_ELEMENT_DESC LayoutDescription[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		uint32 InputCount = ARRAYSIZE(LayoutDescription);
		Result = D3DCompileFromFile(L"Shaders/OverlayRect.fx", 0, 0, "VS", "vs_4_0", 0, 0, &VSBuffer, 0);
		Result = Device->CreateVertexShader(VSBuffer->GetBufferPointer(), VSBuffer->GetBufferSize(), NULL, &m_VertexShader);
		Result = Device->CreateInputLayout(LayoutDescription, InputCount, VSBuffer->GetBufferPointer(), VSBuffer->GetBufferSize(), &m_InputLayout);
		VSBuffer->Release();

		ID3D10Blob* PSBuffer;
		Result = D3DCompileFromFile(L"Shaders/OverlayRect.fx", 0, 0, "PS", "ps_4_0", 0, 0, &PSBuffer, 0);
		Result = Device->CreatePixelShader(PSBuffer->GetBufferPointer(), PSBuffer->GetBufferSize(), NULL, &m_PixelShader);
		PSBuffer->Release();

		D3D11_BUFFER_DESC ConstantBufferDescription = { 0 };
		ConstantBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		ConstantBufferDescription.ByteWidth = sizeof(XMMATRIX);
		ConstantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ConstantBufferDescription.CPUAccessFlags = 0;
		ConstantBufferDescription.MiscFlags = 0;
		Device->CreateBuffer(&ConstantBufferDescription, NULL, &m_TransformBuffer);

		ConstantBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		ConstantBufferDescription.ByteWidth = sizeof(XMFLOAT4);
		ConstantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ConstantBufferDescription.CPUAccessFlags = 0;
		ConstantBufferDescription.MiscFlags = 0;
		Device->CreateBuffer(&ConstantBufferDescription, NULL, &m_ColorBuffer);

		float Vertices[6][2] =
		{
			{ 0.0f, 1.0f },
			{ 1.0f, 0.0f },
			{ 0.0f, 0.0f },
			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f },
			{ 1.0f, 0.0f }
		};

		D3D11_BUFFER_DESC VertexBufferDescription = { 0 };
		VertexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		VertexBufferDescription.ByteWidth = sizeof(Vertices);
		VertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		VertexBufferDescription.CPUAccessFlags = 0;
		VertexBufferDescription.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA VertexBufferData = { 0 };
		VertexBufferData.pSysMem = Vertices;
		Result = Device->CreateBuffer(&VertexBufferDescription, &VertexBufferData, &m_VertexBuffer);

		D3D11_RENDER_TARGET_BLEND_DESC RenderTargetBlendDesc = { 0 };
		RenderTargetBlendDesc.BlendEnable = true;
		RenderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		RenderTargetBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		RenderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
		RenderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
		RenderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
		RenderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		RenderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		D3D11_BLEND_DESC BlendDesc = { 0 };
		BlendDesc.AlphaToCoverageEnable = false;
		BlendDesc.RenderTarget[0] = RenderTargetBlendDesc;
		Result = Device->CreateBlendState(&BlendDesc, &m_BlendState);

		m_Transform = XMMatrixMultiply(XMMatrixScaling(Width, Height, 1.0f), XMMatrixTranslation(X, Y, 2.0f));
	}

	void Render(ID3D11DeviceContext* DeviceContext, XMFLOAT4 Color, XMMATRIX* Projection)
	{
		uint32 Stride[1] = { sizeof(float) * 2 };
		uint32 Offset[1] = { 0 };
		DeviceContext->IASetInputLayout(m_InputLayout);
		DeviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, Stride, Offset);
		DeviceContext->VSSetShader(m_VertexShader, 0, 0);
		DeviceContext->VSSetConstantBuffers(0, 1, &m_TransformBuffer);
		XMMATRIX Transform = DirectX::XMMatrixMultiplyTranspose(m_Transform, *Projection);
		DeviceContext->UpdateSubresource(m_TransformBuffer, 0, NULL, &Transform, 0, 0);
		DeviceContext->PSSetShader(m_PixelShader, 0, 0);
		DeviceContext->PSSetConstantBuffers(0, 1, &m_ColorBuffer);
		DeviceContext->UpdateSubresource(m_ColorBuffer, 0, NULL, &Color, 0, 0);
		float BlendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		DeviceContext->OMSetBlendState(m_BlendState, BlendFactor, 0xFFFFFFFF);
		DeviceContext->Draw(6, 0);
	}

	void SetTransform(XMMATRIX* Transform)
	{
		m_Transform = *Transform;
	}
};