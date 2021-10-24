#pragma once
#include "defs.h"
#include <Windows.h>
#include <d3d11.h>
#include <cstdarg>
#include "camera.h"
#include "shaders.h"

struct test_object;

class render_system
{
public:
	render_system(HWND Window, HINSTANCE Instance);
	~render_system();

	void Render();
private:
	IDXGISwapChain*			m_SwapChain;
	ID3D11Device*			m_Device;
	ID3D11DeviceContext*	m_DeviceContext;
	ID3D11RenderTargetView*	m_MainRenderTargetView;
	ID3D11DepthStencilView* m_DepthStencilView;

	// TEST GARBAGE REMOVE
	void InitScene();
	void RenderTest(test_object* TestObject, DirectX::XMMATRIX VPMatrix);
};