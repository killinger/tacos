#pragma comment(lib, "d3d11.lib")
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include "taco_game.h"
#include "system_event_queue.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>
#include "subsystems.h"
#include "auto_profiler.h"

system_event_queue	SystemEventQueue;

HWND				CreateAndRegisterWindow(HINSTANCE Instance);
LRESULT CALLBACK	ProcessSystemEvents(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam);

int WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPTSTR CmdLine, int CmdShow)
{
	HWND Window = CreateAndRegisterWindow(Instance);
	if (Window)
	{
		MSG Message = { 0 };
		LARGE_INTEGER StartCounter, EndCounter;
		LARGE_INTEGER FrameStart = { 0 };
		LARGE_INTEGER FrameEnd = { 0 };
		LARGE_INTEGER Frequency;
		QueryPerformanceFrequency(&Frequency);
		QueryPerformanceCounter(&StartCounter);
		double TargetFrameTime = 1.0 / 60.0;
#if defined(_PROFILE) && !defined(_DEBUG) 
		TargetFrameTime = 0.0;
#endif // _PROFILE
		double Next = 0.0;
		double Now = 0.0;

		taco::Initialize(Instance, Window, &SystemEventQueue);

#ifdef _PROFILE
		ProfileSystem->m_Frequency = (double)Frequency.QuadPart;
#endif // _PROFILE


		while (true)
		{
			if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) // TODO: Maybe look these up lol
			{
				if (Message.message == WM_QUIT)
					break;
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			}
			
			QueryPerformanceCounter(&EndCounter);
			Now = (double)(EndCounter.QuadPart - StartCounter.QuadPart) / (double)Frequency.QuadPart;
			if (Now >= Next)
			{
				taco::RunFrame();
#ifdef _PROFILE
				QueryPerformanceCounter(&FrameEnd);
				ProfileSystem->m_FrameTime = ((double)(FrameEnd.QuadPart - FrameStart.QuadPart) / (double)Frequency.QuadPart) * 1000.0;
				QueryPerformanceCounter(&FrameStart);
#endif // _PROFILE
				Next = Now + TargetFrameTime;
			}
		}
	}
	return 0;
}

HWND CreateAndRegisterWindow(HINSTANCE Instance)
{
	WNDCLASSEX WindowClass = { 0 };
	HWND Window = NULL;
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = ProcessSystemEvents;
	WindowClass.cbClsExtra = NULL;
	WindowClass.cbWndExtra = NULL;
	WindowClass.hInstance = Instance;
	WindowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WindowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	WindowClass.lpszMenuName = NULL;
	WindowClass.lpszClassName = "Window";
	WindowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&WindowClass))
		return NULL;

	RECT Rect;
	Rect.left = 0;
	Rect.top = 0;
	Rect.right = 1280;
	Rect.bottom = 720;
	AdjustWindowRect(&Rect, WS_OVERLAPPEDWINDOW, false);

	int b = 2;
	Window = CreateWindowEx(NULL,
							"Window",	// window class
							"tacos",	// window name
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							Rect.right - Rect.left, // width
							Rect.bottom - Rect.top, // height
							NULL,
							NULL,
							Instance,
							NULL);
	ShowWindow(Window, true);
	UpdateWindow(Window);
	return Window;
}

LRESULT CALLBACK ProcessSystemEvents(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
	switch (Message)
	{
	case WM_KEYDOWN:
		if (WParam == VK_ESCAPE)
		{
			DestroyWindow(Window);
			return 0;
		}
		else
		{
			SystemEventQueue.Enqueue(EVENT_KEY, WParam & 255, true);
			break;
		}
		break;
	case WM_CHAR:
		SystemEventQueue.Enqueue(EVENT_CHAR, WParam, 0);
		break;
	case WM_LBUTTONDOWN:
		SystemEventQueue.Enqueue(EVENT_MOUSEDOWN, GET_X_LPARAM(LParam), GET_Y_LPARAM(LParam));
		break;
	case WM_RBUTTONDOWN:
		SystemEventQueue.Enqueue(EVENT_MOUSE2DOWN, GET_X_LPARAM(LParam), GET_Y_LPARAM(LParam));
		break;
	case WM_LBUTTONUP:
		SystemEventQueue.Enqueue(EVENT_MOUSEUP, GET_X_LPARAM(LParam), GET_Y_LPARAM(LParam));
		break;
	case WM_MOUSEMOVE:
		SystemEventQueue.Enqueue(EVENT_MOUSEMOVE, GET_X_LPARAM(LParam), GET_Y_LPARAM(LParam));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(Window, Message, WParam, LParam);
}