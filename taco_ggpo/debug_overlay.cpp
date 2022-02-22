#include "debug_overlay.h"
#include "subsystems.h"
#include "render_system.h"
#include "auto_profiler.h"

debug_window::debug_window(float X, float Y, std::string Label, window_type_function* StringFunction)
{
	Initialize(X, Y, Label, StringFunction);
}

void debug_window::Initialize(float X, float Y, std::string Label, window_type_function* StringFunction)
{
	StringFunction->SetDimensions(m_Dimensions);
	m_Position.x = X;
	m_Position.y = Y;
	m_TopPosition.x = X;
	m_TopPosition.y = Y + m_Dimensions.y + 4.0f;
	m_TopDimensions.x = m_Dimensions.x ;
	m_TopDimensions.y = 16.0f;
	m_CloseButtonPosition.x = m_Position.x + m_TopDimensions.x + 1.0f;
	m_CloseButtonPosition.y = m_TopPosition.y;
	m_CloseButtonDimensions.x = m_Dimensions.x - m_TopDimensions.x - 1.0f;
	m_CloseButtonDimensions.y = m_TopDimensions.y;
	m_WindowColor = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.65f);
	m_MouseOverColor = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.45f);
	m_TopColor = XMFLOAT4(0.12f, 0.12f, 0.12f, 0.85f);
	m_CloseButtonColor = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.65f);
	m_PrevCursorPos.x = 0.0f;
	m_PrevCursorPos.y = 0.0f;
	m_StringFunction = StringFunction;
	m_Label = Label;
	UpdateTransforms();
}

void debug_window::Render(gamestate* GameState, state_manager* StateManager)
{
	if (m_Mode == MODE_MOUSE_OVER || m_Mode == MODE_DRAG)
		RenderSystem->DrawRectangle(&m_Transform, m_MouseOverColor);
	else
		RenderSystem->DrawRectangle(&m_Transform, m_WindowColor);
	RenderSystem->DrawRectangle(&m_TopTransform, m_TopColor);
	RenderSystem->DrawStringSmall(m_Position.x, m_TopPosition.y + 2.0f, 1.0f, m_Label.c_str());
	RenderSystem->DrawRectangle(&m_CloseButtonTransform, m_CloseButtonColor);
	m_StringFunction->DrawStrings(m_Position.x + 2.0f, m_Position.y + m_Dimensions.y - (float)SMALL_TEXT_SIZE, GameState, StateManager);
}

bool debug_window::MouseEvent(system_event Event)
{
	switch (Event.Type)
	{
	case EVENT_MOUSEDOWN:
		if (HitTestTop(Event.Parameters[0], Event.Parameters[1]))
		{
			m_Mode = MODE_DRAG;
			return true;
		}
		break;
	case EVENT_MOUSEUP:
		if (m_Mode == MODE_DRAG)
		{
			m_Mode = MODE_MOUSE_OVER;
			return true;
		}
		break;
	case EVENT_MOUSEMOVE:
		if (m_Mode == MODE_DRAG)
		{
			MoveWindow(Event.Parameters[0], Event.Parameters[1]);
			return true;
		}
		else
		{
			if (HitTestBody(Event.Parameters[0], Event.Parameters[1]))
			{
				m_Mode = MODE_MOUSE_OVER;
				return true;
			}
			m_Mode = MODE_NORMAL;
		}
		break;
	default:
		break;
	}
	return false;
}

bool debug_window::HitTestTop(int32 X, int32 Y)
{
	float XF = (float)X - (1280.0f / 2.0f);
	float YF = 720.0f - (float)Y - (720.0f / 2.0f);

	if ((XF >= m_TopPosition.x && XF <= (m_TopPosition.x + m_TopDimensions.x)) &&
		(YF >= m_TopPosition.y && YF <= (m_TopPosition.y + m_TopDimensions.y)))
	{
		m_PrevCursorPos.x = XF;
		m_PrevCursorPos.y = YF;
		return true;
	}
	return false;
}

bool debug_window::HitTestBody(int32 X, int32 Y)
{
	float XF = (float)X - (1280.0f / 2.0f);
	float YF = 720.0f - (float)Y - (720.0f / 2.0f);

	if ((XF >= m_Position.x && XF <= (m_Position.x + m_TopDimensions.x)) &&
		(YF >= m_Position.y && YF <= (m_TopPosition.y + m_TopDimensions.y)))
	{
		m_PrevCursorPos.x = XF;
		m_PrevCursorPos.y = YF;
		return true;
	}
	return false;
}

void debug_window::MoveWindow(int32 X, int32 Y)
{
	float XF = (float)X - (1280.0f / 2.0f);
	float YF = 720.0f - (float)Y - (720.0f / 2.0f);
	float dX = XF - m_PrevCursorPos.x;
	float dY = YF - m_PrevCursorPos.y;
	m_Position.x += dX;
	m_Position.y += dY;
	m_TopPosition.x += dX;
	m_TopPosition.y += dY;
	m_CloseButtonPosition.x += dX;
	m_CloseButtonPosition.y += dY;
	m_PrevCursorPos.x = XF;
	m_PrevCursorPos.y = YF;
	UpdateTransforms();
}

void debug_window::UpdateTransforms()
{
	m_Transform = XMMatrixScaling(m_Dimensions.x, m_Dimensions.y, 1.0f) * XMMatrixTranslation(m_Position.x, m_Position.y, 2.0f);
	m_TopTransform = XMMatrixScaling(m_TopDimensions.x, m_TopDimensions.y, 1.0f) * XMMatrixTranslation(m_TopPosition.x, m_TopPosition.y, 2.0f);
	m_CloseButtonTransform = XMMatrixScaling(m_CloseButtonDimensions.x, m_CloseButtonDimensions.y, 1.0f) * XMMatrixTranslation(m_CloseButtonPosition.x, m_CloseButtonPosition.y, 5.1f);
}

overlay_manager::overlay_manager()
{
	m_Position.x = -640.0f;
	m_Position.y = 347.0f;
	m_Dimensions.x = 67.5f;
	m_Dimensions.y = 13.0f;
	m_ButtonColor = XMFLOAT4(0.12f, 0.12f, 0.12f, 0.85f);
	m_MouseOverColor = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.45f);
	m_DropdownOpenColor = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.45f);
	m_DropdownEntryColor = XMFLOAT4(0.12f, 0.12f, 0.12f, 0.85f);
	m_DropdownEntryMouseOverColor = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.45f);
	m_DropdownPosition.x = m_Position.x + m_Dimensions.x;
	m_DropdownPosition.y = m_Position.y;
	m_State = STATE_NONE;
	m_PrevCursorPos.x = 0.0f;
	m_PrevCursorPos.y = 0.0f;
	// TODO: delete those pointers
#ifdef _PROFILE
	AddDropdownEntry("Profiling", new window_function_profiling());
#endif
	AddDropdownEntry("P1 state information", new window_function_p1_state());
	AddDropdownEntry("P2 state information", new window_function_p2_state());
	AddDropdownEntry("P1 properties", new window_function_p1_properties());
	AddDropdownEntry("P2 properties", new window_function_p2_properties());
	UpdateTransform();
}

void overlay_manager::AddDropdownEntry(const char* Label, window_type_function* Click)
{
	m_DropdownEntries.push_back(dropdown_entry(m_DropdownPosition.x, m_DropdownPosition.y, 130.0f, m_Dimensions.y, Label, Click));
	m_DropdownPosition.y -= m_Dimensions.y;
}

void overlay_manager::MouseEvent(system_event Event)
{
	XMFLOAT2 Location;
	switch (Event.Type)
	{
	case EVENT_MOUSEDOWN:
		if (m_State == STATE_DROP_DOWN_OPEN)
		{
			for (uint32 i = 0; i < m_DropdownEntries.size(); i++)
			{
				if (m_DropdownEntries[i].HitTest(Event.Parameters[0], Event.Parameters[1]))
				{
					m_Windows.push_back(debug_window(0.0f, 0.0f, m_DropdownEntries[i].m_Label, m_DropdownEntries[i].OnClick()));
					break;
				}
			}
			m_State = STATE_NONE;
		}
		else if (HitTest(Event.Parameters[0], Event.Parameters[1], Location))
		{
			m_State = STATE_DROP_DOWN_OPEN;
		}
		else
		{
			m_State = STATE_NONE;
			for (uint32 i = 0; i < m_Windows.size(); i++)
			{
				if (m_Windows[i].MouseEvent(Event))
					break;
			}
		}
		break;
	case EVENT_MOUSE2DOWN:
		for (uint32 i = 0; i < m_Windows.size(); i++)
		{
				if (m_Windows[i].HitTestBody(Event.Parameters[0], Event.Parameters[1]))
				{
					m_Windows.erase(m_Windows.begin() + i);
					break;
				}
		}
		break;
	case EVENT_MOUSEUP:
		for (uint32 i = 0; i < m_Windows.size(); i++)
		{
			if (m_Windows[i].MouseEvent(Event))
					break;
		}
		break;
	case EVENT_MOUSEMOVE:
		if (m_State == STATE_DROP_DOWN_OPEN)
		{
			for (uint32 i = 0; i < m_DropdownEntries.size(); i++)
			{
				if (m_DropdownEntries[i].HitTest(Event.Parameters[0], Event.Parameters[1]))
					m_DropdownEntries[i].m_MouseOver = true;
				else
					m_DropdownEntries[i].m_MouseOver = false;
			}
		}
		else if (HitTest(Event.Parameters[0], Event.Parameters[1], Location))
		{
			m_State = STATE_MOUSE_OVER;
		}
		else
		{
			m_State = STATE_NONE;
			for (uint32 i = 0; i < m_Windows.size(); i++)
			{
				if (m_Windows[i].MouseEvent(Event))
					break;
			}
		}
		break;
	default:
		break;
	}
}

bool overlay_manager::HitTest(int32 X, int32 Y, XMFLOAT2& Location)
{
	float XF = (float)X - (1280.0f / 2.0f);
	float YF = 720.0f - (float)Y - (720.0f / 2.0f);

	if ((XF >= m_Position.x && XF <= (m_Position.x + m_Dimensions.x)) &&
		(YF >= m_Position.y && YF <= (m_Position.y + m_Dimensions.y)))
	{
		Location.x = XF;
		Location.y = YF;
		return true;
	}
	return false;
}

void overlay_manager::Render(gamestate* GameState, state_manager* StateManager)
{
	PROFILE();
	if (m_State == STATE_NONE || m_State == STATE_DROP_DOWN_OPEN)
		RenderSystem->DrawRectangle(&m_Transform, m_ButtonColor);
	else if (m_State == STATE_MOUSE_OVER)
		RenderSystem->DrawRectangle(&m_Transform, m_MouseOverColor);
	RenderSystem->DrawStringSmall(m_Position.x, m_Position.y + 2.0f, 1.0f, ">debug menu");

	if (m_State == STATE_DROP_DOWN_OPEN)
	{
		for (uint32 i = 0; i < m_DropdownEntries.size(); i++)
		{
			if (m_DropdownEntries[i].m_MouseOver)
				RenderSystem->DrawRectangle(&m_DropdownEntries[i].m_Transform, m_DropdownEntryMouseOverColor);
			else
				RenderSystem->DrawRectangle(&m_DropdownEntries[i].m_Transform, m_DropdownEntryColor);
			RenderSystem->DrawStringSmall(m_DropdownEntries[i].m_Position.x, m_DropdownEntries[i].m_Position.y + 2.0f, 1.0f, m_DropdownEntries[i].m_Label.c_str());
		}
	}

	for (uint32 i = 0; i < m_Windows.size(); i++)
	{
			m_Windows[i].Render(GameState, StateManager);
	}
}

void overlay_manager::UpdateTransform()
{
	m_Transform = XMMatrixScaling(m_Dimensions.x, m_Dimensions.y, 1.0f) * XMMatrixTranslation(m_Position.x, m_Position.y, 2.0f);
}

void window_function_p1_state::SetDimensions(XMFLOAT2& Dimensions)
{
	Dimensions.x = 138.0f;
	Dimensions.y = 100.0f;
}

void window_function_p1_state::DrawStrings(float X, float Y, gamestate* GameState, state_manager* StateManager)
{
	state_script* Script = StateManager->GetScript(GameState->m_Player[0].PlaybackState.State);
	RenderSystem->DrawStringSmall(X, Y, 1.0f,
		"State\n  %s\nCursor position\n  %d / %d\nPlayback rate\n  %.2f\nHitstop\n  %d",
		Script->Name.c_str(), GameState->m_Player[0].PlaybackState.PlaybackCursor + 1, Script->TotalFrames, GameState->m_Player[0].PlaybackState.PlaybackRate,
		GameState->m_Player[0].Hitstop);
}

void window_function_p2_state::SetDimensions(XMFLOAT2& Dimensions)
{
	Dimensions.x = 138.0f;
	Dimensions.y = 100.0f;
}

void window_function_p2_state::DrawStrings(float X, float Y, gamestate* GameState, state_manager* StateManager)
{
	state_script* Script = StateManager->GetScript(GameState->m_Player[1].PlaybackState.State);
	RenderSystem->DrawStringSmall(X, Y, 1.0f,
		"State\n  %s\nCursor position\n  %d / %d\nPlayback rate\n  %.2f\nHitstop\n  %d",
		Script->Name.c_str(), GameState->m_Player[1].PlaybackState.PlaybackCursor + 1, Script->TotalFrames, GameState->m_Player[1].PlaybackState.PlaybackRate,
		GameState->m_Player[1].Hitstop);
}

void window_function_p1_properties::SetDimensions(XMFLOAT2& Dimensions)
{
	Dimensions.x = 138.0f;
	Dimensions.y = 76.0f;
}

void window_function_p1_properties::DrawStrings(float X, float Y, gamestate* GameState, state_manager* StateManager)
{
	RenderSystem->DrawStringSmall(X, Y, 1.0f, 
		"Position\n  %.2f, %.2f\nVelocity\n  %.2f, %.2f\nAcceleration\n  %.2f, %.2f", 
		GameState->m_Player[0].PositionX, GameState->m_Player[0].PositionY,
		GameState->m_Player[0].VelocityX, GameState->m_Player[0].VelocityY,
		GameState->m_Player[0].AccelerationX, GameState->m_Player[0].AccelerationY);
}

void window_function_p2_properties::SetDimensions(XMFLOAT2& Dimensions)
{
	Dimensions.x = 138.0f;
	Dimensions.y = 76.0f;
}

void window_function_p2_properties::DrawStrings(float X, float Y, gamestate* GameState, state_manager* StateManager)
{
	RenderSystem->DrawStringSmall(X, Y, 1.0f,
		"Position\n  %.2f, %.2f\nVelocity\n  %.2f, %.2f\nAcceleration\n  %.2f, %.2f",
		GameState->m_Player[1].PositionX, GameState->m_Player[1].PositionY,
		GameState->m_Player[1].VelocityX, GameState->m_Player[1].VelocityY,
		GameState->m_Player[1].AccelerationX, GameState->m_Player[1].AccelerationY);
}

#ifdef _PROFILE
void window_function_profiling::SetDimensions(XMFLOAT2& Dimensions)
{
	Dimensions.x = 700.0f;
	Dimensions.y = 211.0f;
}

void DrawStringRecursive(float X, float &Y, profile_node* CurrentNode, double Baseline, float StartX)
{
	
}

void window_function_profiling::DrawStrings(float X, float Y, gamestate* GameState, state_manager* StateManager)
{
	RenderSystem->DrawStringSmall(X + 560.0f, Y, 1.0f,
		"%f", ProfileSystem->m_Root.m_AverageTime);
	Y -= 12.0f;
	for (uint32 i = 0; i < ProfileSystem->m_NodeList.size(); i++)
	{
		RenderSystem->DrawString(X, Y, 1.0f,
			"%s", ProfileSystem->m_NodeList[i]->m_Name);
		int32 Cursor = ProfileSystem->m_NodeList[i]->m_HistoryCursor;
		float XOffset = 500.0f;
		for (uint32 j = 0; j < PROFILE_HISTORY_SIZE; j++)
		{
			if (--Cursor < 0)
				Cursor = PROFILE_HISTORY_SIZE - 1;
			float Scaling = (float)ProfileSystem->m_NodeList[i]->m_History[Cursor];
			XMMATRIX Transform = XMMatrixScaling(3.0f, 21.0f * Scaling, 1.0f) * XMMatrixTranslation(X + XOffset, Y, 2.0f);
			RenderSystem->DrawRectangle(&Transform, XMFLOAT4(Scaling, 1.0f - Scaling, 0.0f, 1.0f));
			XOffset -= 3.0f;
		}
		Y -= 22.0f;
	}
}
#endif