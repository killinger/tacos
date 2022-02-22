#pragma once
#include "defs.h"
#include <Windows.h>
#include <d3d11.h>
#include <cstdarg>
#include "camera.h"
#include "shaders.h"
#include "d3d11_font.h"
#include "d3d11_rectangle.h"
#include "render_state.h"

#define SMALL_TEXT_SIZE 12

struct test_object;
struct test_object2;

class scene_graph;
class character_node;

class render_system
{
public:
	render_system(HWND Window, HINSTANCE Instance);
	~render_system();

	void Clear();
	void Render(camera* Camera);
	void Render(scene_graph* Scene);
	void Present();
	void DrawString(float XOffset, float YOffset, float Scale, const char* String...);
	void DrawStringSmall(float XOffset, float YOffset, float Scale, const char* String...);
	void DrawStringSmallRightAligned(float XOffset, float YOffset, float Scale, const char* String...);
	void DrawRectangle(XMMATRIX* Transform, XMFLOAT4 Color);
private:
	struct textured_quad
	{
		ID3D11Buffer*		m_VertexBuffer; // TODO: Move these
		ID3D11VertexShader* m_VertexShader;	// TODO: Move these. Use the rectangle. Also use same shaders as the rectangle?
		ID3D11PixelShader*	m_PixelShader;	// TODO: Move these
	};

	IDXGISwapChain*			m_SwapChain;
	ID3D11Device*			m_Device;
	ID3D11DeviceContext*	m_DeviceContext;
	ID3D11RenderTargetView*	m_MainRenderTargetView;
	ID3D11DepthStencilView* m_DepthStencilView;
	DirectX::XMMATRIX		m_OverlayTransform;
	d3d11_font				m_FontMedium;
	d3d11_font				m_FontSmall;
	d3d11_rectangle			m_Rectangle;
	textured_quad			m_TexturedQuad;

	// TODO
	render_state_character	m_RenderStateCharacter;

	// TEST GARBAGE REMOVE
	void InitScene();
	void InitRenderStates();
	void CharacterOffscreenPass(character_node* Node, ID3D11RenderTargetView* RenderTarget);
	void RenderTest(test_object* TestObject, DirectX::XMMATRIX VPMatrix);
	void RenderTest(test_object2* TestObject, DirectX::XMMATRIX VPMatrix);
	void RenderText(d3d11_font* Font, float XOffset, float YOffset, float Scale, const char* String);
	void RenderTextRightAligned(d3d11_font* Font, float XOffset, float YOffset, float Scale, const char* String);
};