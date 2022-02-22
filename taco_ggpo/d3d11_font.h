#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H

struct d3d11_character
{
	ID3D11ShaderResourceView*	m_ShaderResourceView;
	int32						m_Width;
	int32						m_Height;
	int32						m_BearingX;
	int32						m_BearingY;
	uint32						m_Advance;
};

// TODO: Use a font library that creates a texture atlas, or maybe just create one meself?
struct d3d11_font
{
	d3d11_character		m_Characters[128];
	ID3D11SamplerState* m_SamplerState;	// TODO: Move these
	ID3D11Buffer*		m_VertexBuffer; // TODO: Move these
	ID3D11VertexShader* m_VertexShader;	// TODO: Move these. Use the rectangle. Also use same shaders as the rectangle?
	ID3D11PixelShader*	m_PixelShader;	// TODO: Move these
	ID3D11InputLayout*	m_InputLayout;	// TODO: Move these
	ID3D11Buffer*		m_ProjectionBuffer; // TODO: Move these?
	ID3D11Buffer*		m_ColorBuffer;		// TODO: Move these?
	ID3D11BlendState*	m_BlendState;	// TODO: Move these
	uint32				m_PixelWidth;
	uint32				m_PixelHeight;

	void InitializeFromFile(const char* Filename, uint32 PixelWidth, uint32 PixelHeight, ID3D11Device* Device)
	{
		FT_Library FtLib;
		// TODO: Error checking
		if (FT_Init_FreeType(&FtLib))
		{
			int b = 2;
		}

		FT_Face Face;
		if (FT_New_Face(FtLib, Filename, 0, &Face))
		{
			int b = 2;
		}
		FT_Set_Pixel_Sizes(Face, PixelWidth, PixelHeight);
		m_PixelWidth = PixelWidth;
		m_PixelHeight = PixelHeight;

		HRESULT Result;
		ID3D10Blob* VSBuffer = NULL;
		D3D11_INPUT_ELEMENT_DESC LayoutDescription[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		uint32 InputCount = ARRAYSIZE(LayoutDescription);
		Result = D3DCompileFromFile(L"Shaders/Text.fx", 0, 0, "VS", "vs_4_0", 0, 0, &VSBuffer, 0);
		Result = Device->CreateVertexShader(VSBuffer->GetBufferPointer(), VSBuffer->GetBufferSize(), NULL, &m_VertexShader);
		Result = Device->CreateInputLayout(LayoutDescription, InputCount, VSBuffer->GetBufferPointer(), VSBuffer->GetBufferSize(), &m_InputLayout);
		VSBuffer->Release();

		ID3D10Blob* PSBuffer;
		Result = D3DCompileFromFile(L"Shaders/Text.fx", 0, 0, "PS", "ps_4_0", 0, 0, &PSBuffer, 0);
		Result = Device->CreatePixelShader(PSBuffer->GetBufferPointer(), PSBuffer->GetBufferSize(), NULL, &m_PixelShader);
		PSBuffer->Release();

		D3D11_BUFFER_DESC ConstantBufferDescription = { 0 };
		ConstantBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		ConstantBufferDescription.ByteWidth = sizeof(DirectX::XMMATRIX);
		ConstantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ConstantBufferDescription.CPUAccessFlags = 0;
		ConstantBufferDescription.MiscFlags = 0;
		Device->CreateBuffer(&ConstantBufferDescription, NULL, &m_ProjectionBuffer);

		ConstantBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		ConstantBufferDescription.ByteWidth = sizeof(DirectX::XMFLOAT4);
		ConstantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ConstantBufferDescription.CPUAccessFlags = 0;
		ConstantBufferDescription.MiscFlags = 0;
		Device->CreateBuffer(&ConstantBufferDescription, NULL, &m_ColorBuffer);

		// TODO: Use the already buffered vertices for the rectangle
		float Vertices[6][4] =
		{
			{ 0.0f, 1.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 1.0f, 1.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f, 0.0f },
			{ 1.0f, 0.0f, 1.0f, 1.0f },
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

		D3D11_SAMPLER_DESC SamplerDescription = { };
		SamplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;
		SamplerDescription.MinLOD = 0;
		SamplerDescription.MaxLOD = D3D11_FLOAT32_MAX;
		Result = Device->CreateSamplerState(&SamplerDescription, &m_SamplerState);

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

		for (unsigned char c = 0; c < 128; c++)
		{
			if (FT_Load_Char(Face, c, FT_LOAD_RENDER))
			{
				int b = 2;
			}

			D3D11_SUBRESOURCE_DATA TextureData;
			TextureData.pSysMem = Face->glyph->bitmap.buffer;
			TextureData.SysMemPitch = Face->glyph->bitmap.width;
			TextureData.SysMemSlicePitch = 0;

			D3D11_TEXTURE2D_DESC TextureDescription = { 0 };
			TextureDescription.Width = Face->glyph->bitmap.width;
			TextureDescription.Height = Face->glyph->bitmap.rows;
			TextureDescription.MipLevels = 1;
			TextureDescription.ArraySize = 1;
			TextureDescription.Format = DXGI_FORMAT_R8_UNORM;
			TextureDescription.SampleDesc.Count = 1;
			TextureDescription.SampleDesc.Quality = 0;
			TextureDescription.Usage = D3D11_USAGE_DEFAULT;
			TextureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			TextureDescription.CPUAccessFlags = 0;
			TextureDescription.MiscFlags = 0;

			ID3D11Texture2D* Texture = NULL;
			Result = Device->CreateTexture2D(&TextureDescription, &TextureData, &Texture);

			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDescription = { };
			SRVDescription.Format = DXGI_FORMAT_R8_UNORM;
			SRVDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVDescription.Texture2D.MostDetailedMip = 0;
			SRVDescription.Texture2D.MipLevels = -1;
			Result = Device->CreateShaderResourceView(Texture, &SRVDescription, &m_Characters[c].m_ShaderResourceView);

			m_Characters[c].m_Width = Face->glyph->bitmap.width;
			m_Characters[c].m_Height = Face->glyph->bitmap.rows;
			m_Characters[c].m_BearingX = Face->glyph->bitmap_left;
			m_Characters[c].m_BearingY = Face->glyph->bitmap_top;
			m_Characters[c].m_Advance = Face->glyph->advance.x;
		}
		FT_Done_Face(Face);
		FT_Done_FreeType(FtLib);
	}

	uint32 GetStringPixelWidth(const char* String)
	{
		uint32 Width = 0;
		uint32 StringLength = strlen(String);
		for (uint32 i = 0; i < StringLength - 1; i++)
		{
			d3d11_character* Character = &m_Characters[String[i]];
			Width += Character->m_Advance >> 6;
		}
		Width += m_Characters[String[StringLength - 1]].m_BearingX + m_Characters[String[StringLength - 1]].m_Width;
		return Width;
	}

	void Release()
	{
		for (uint32 i = 0; i < 128; i++)
			m_Characters[i].m_ShaderResourceView->Release();
		m_SamplerState->Release();
		m_VertexBuffer->Release();
		m_PixelShader->Release();
		m_InputLayout->Release();
		m_ProjectionBuffer->Release();
		m_ColorBuffer->Release();
		m_BlendState->Release();
	}
};