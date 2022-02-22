#pragma once

struct render_state
{
	ID3D11VertexShader*		m_VertexShader;
	ID3D11InputLayout*		m_InputLayout;
	ID3D11PixelShader*		m_PixelShader;
	ID3D11Buffer*			m_CbPerObject;
	ID3D11Buffer*			m_CbPerFrame;
};

struct render_state_character
{
	render_state				m_OffscreenPass;
	render_state				m_OutlinePass;
	render_state				m_SpritePass;
	ID3D11RenderTargetView*		m_OffScreenTarget[2];
	ID3D11ShaderResourceView*	m_SpriteTexture[2];
	ID3D11RasterizerState*		m_OutlineRasterizer;
	ID3D11SamplerState*			m_SpriteSampler;
	ID3D11DepthStencilView*		m_OffscreenDepthView;
};