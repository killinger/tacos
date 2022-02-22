#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include "defs.h"
#include "system_event.h"
#include "gamestate.h"
#include <vector>

using namespace DirectX;

class window_type_function
{
public:
	void virtual SetDimensions(XMFLOAT2& Dimensions) = 0;
	void virtual DrawStrings(float X, float Y, gamestate* GameState, state_manager* StateManager) = 0;
};

class window_function_p1_state : public window_type_function
{
public:
	void virtual SetDimensions(XMFLOAT2& Dimensions) override;
	void virtual DrawStrings(float X, float Y, gamestate* GameState, state_manager* StateManager) override;
};

class window_function_p2_state : public window_type_function
{
public:
	void virtual SetDimensions(XMFLOAT2& Dimensions) override;
	void virtual DrawStrings(float X, float Y, gamestate* GameState, state_manager* StateManager) override;
};

class window_function_p1_properties : public window_type_function
{
public:
	void virtual SetDimensions(XMFLOAT2& Dimensions) override;
	void virtual DrawStrings(float X, float Y, gamestate* GameState, state_manager* StateManager) override;
};

class window_function_p2_properties : public window_type_function
{
public:
	void virtual SetDimensions(XMFLOAT2& Dimensions) override;
	void virtual DrawStrings(float X, float Y, gamestate* GameState, state_manager* StateManager) override;
};

#ifdef _PROFILE
class window_function_profiling : public window_type_function
{
public:
	void virtual SetDimensions(XMFLOAT2& Dimensions) override;
	void virtual DrawStrings(float X, float Y, gamestate* GameState, state_manager* StateManager) override;
};
#endif

struct debug_window
{
	debug_window(float X, float Y, std::string Label, window_type_function* StringFunction);

	XMMATRIX				m_Transform;
	XMMATRIX				m_TopTransform;
	XMMATRIX				m_CloseButtonTransform;
	XMFLOAT4				m_WindowColor;
	XMFLOAT4				m_MouseOverColor;
	XMFLOAT4				m_CloseButtonColor;
	XMFLOAT4				m_TopColor;
	XMFLOAT2				m_Position;
	XMFLOAT2				m_Dimensions;
	XMFLOAT2				m_TopPosition;
	XMFLOAT2				m_TopDimensions;
	XMFLOAT2				m_CloseButtonPosition;
	XMFLOAT2				m_CloseButtonDimensions;
	XMFLOAT2				m_PrevCursorPos;
	std::string				m_Label;
	window_type_function*	m_StringFunction;
	uint32					m_Mode;

	enum
	{
		MODE_NORMAL,
		MODE_DRAG,
		MODE_MOUSE_OVER
	};

	void Initialize(float X, float Y, std::string Label, window_type_function* StringFunction);
	void Render(gamestate* GameState, state_manager* StateManager);
	bool MouseEvent(system_event Event);
	bool HitTestBody(int32 X, int32 Y);
private:
	void MoveWindow(int32 X, int32 Y);
	bool HitTestTop(int32 X, int32 Y);
	void UpdateTransforms();
};


struct dropdown_entry
{
	dropdown_entry() { }
	dropdown_entry(float X, float Y, float Width, float Height, const char* Label, window_type_function* Click) :
		m_Position(X, Y),
		m_Dimensions(Width, Height),
		m_Label(Label),
		m_ClickReturnFunction(Click),
		m_MouseOver(false) { UpdateTransform(); }
	XMMATRIX				m_Transform;
	XMFLOAT2				m_Position;
	XMFLOAT2				m_Dimensions;
	window_type_function*	m_ClickReturnFunction;
	bool					m_MouseOver;
	std::string m_Label;
	
	void					UpdateTransform()
	{
		m_Transform = XMMatrixScaling(m_Dimensions.x, m_Dimensions.y, 1.0f) * XMMatrixTranslation(m_Position.x, m_Position.y, 2.0f);
	}
	window_type_function* OnClick()
	{
		return m_ClickReturnFunction;
	}
	bool					HitTest(int32 X, int32 Y)
	{
		float XF = (float)X - (1280.0f / 2.0f);
		float YF = 720.0f - (float)Y - (720.0f / 2.0f);

		if ((XF >= m_Position.x && XF <= (m_Position.x + m_Dimensions.x)) &&
			(YF >= m_Position.y && YF <= (m_Position.y + m_Dimensions.y)))
		{
			return true;
		}
		return false;
	}
};

class overlay_manager
{
public:
	overlay_manager();
	void MouseEvent(system_event Event);
	void Render(gamestate* GameState, state_manager* StateManager);
private:
	enum
	{
		STATE_NONE,
		STATE_MOUSE_OVER,
		STATE_DROP_DOWN_OPEN
	};
	XMMATRIX					m_Transform;
	std::vector<debug_window>	m_Windows;
	std::vector<dropdown_entry>	m_DropdownEntries;
	XMFLOAT4					m_ButtonColor;
	XMFLOAT4					m_MouseOverColor;
	XMFLOAT4					m_DropdownOpenColor;
	XMFLOAT4					m_DropdownEntryColor;
	XMFLOAT4					m_DropdownEntryMouseOverColor;
	XMFLOAT2					m_Position;
	XMFLOAT2					m_DropdownPosition;
	XMFLOAT2					m_Dimensions;
	XMFLOAT2					m_PrevCursorPos;
	uint32						m_State;
	void UpdateTransform();
	bool HitTest(int32 X, int32 Y, XMFLOAT2& Location);
	void AddDropdownEntry(const char* Label, window_type_function* Click);
};