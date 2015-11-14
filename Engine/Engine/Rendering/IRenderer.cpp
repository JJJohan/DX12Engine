#include "IRenderer.h"
#include "../Utils/Logging.h"

namespace Engine
{
	HWND IRenderer::_windowClosed = nullptr;

	IRenderer::IRenderer()
		: _screenWidth(0)
		, _screenHeight(0)
		, _aspectRatio(1.0f)
		, _windowed(false)
		, _windowHandle(nullptr)
		, _windowInstance(HINSTANCE(GetModuleHandle(nullptr)))
		, _vsync(true)
	{
		_windowClosed = nullptr;
	    SetClearColour(Colour::Blue);
	}

	IRenderer::~IRenderer()
	{
		
	}

	void IRenderer::SetClearColour(Colour colour)
	{
		_clearColour[0] = colour.GetRed();
		_clearColour[1] = colour.GetGreen();
		_clearColour[2] = colour.GetBlue();
		_clearColour[3] = colour.GetAlpha();
	}

	bool IRenderer::Render()
	{
		// Main message loop:
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return (_windowClosed == _windowHandle);
	}

	bool IRenderer::InitWindow(int width, int height, bool windowed)
	{
		_screenWidth = width;
		_screenHeight = height;
		_aspectRatio = float(width) / float(height);
		_windowed = windowed;

		if (RegisterInstance() == 0)
		{
			Logging::LogWin32Error();
			return EXIT_FAILURE;
		}

		if (RegisterWindow() != 0)
		{
			Logging::LogWin32Error();
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}

	LRESULT CALLBACK IRenderer::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CLOSE:
			DestroyWindow(hwnd);
			_windowClosed = hwnd;
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}

		return EXIT_SUCCESS;
	}

	void IRenderer::SetVsync(bool enabled)
	{
		_vsync = enabled;
	}

	void IRenderer::SetWindowTitle(std::string title)
	{
		if (_windowHandle != nullptr)
		{
			SetWindowText(_windowHandle, title.c_str());
		}
	}

	bool IRenderer::RegisterInstance()
	{
		WNDCLASSEX wcx;
		wcx.cbSize = sizeof(wcx); // size of structure 
		wcx.style = CS_HREDRAW | CS_VREDRAW; // redraw if size changes 
		wcx.lpfnWndProc = WndProc; // points to window procedure 
		wcx.cbClsExtra = 0; // no extra class memory 
		wcx.cbWndExtra = 0; // no extra window memory 
		wcx.hInstance = _windowInstance; // handle to instance 
		wcx.hIcon = LoadIcon(nullptr, IDI_APPLICATION); // predefined app. icon 
		wcx.hCursor = LoadCursor(nullptr, IDC_ARROW); // predefined arrow 
		wcx.hbrBackground = HBRUSH(GetStockObject(BLACK_BRUSH)); // white background brush 
		wcx.lpszMenuName = nullptr; // name of menu resource 
		wcx.lpszClassName = "EngineProcess"; // name of window class 
		wcx.hIconSm = HICON(LoadImage(_windowInstance, // small class icon 
			MAKEINTRESOURCE(5),
			IMAGE_ICON,
			GetSystemMetrics(SM_CXSMICON),
			GetSystemMetrics(SM_CYSMICON),
			LR_DEFAULTCOLOR));

		return RegisterClassEx(&wcx) != 0;
	}

	bool IRenderer::RegisterWindow()
	{
		// Create the main window. 
		_windowHandle = CreateWindow("EngineProcess", "Engine Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
			_screenWidth, _screenHeight, nullptr, nullptr, _windowInstance,	nullptr);

		if (!_windowHandle)
		{
			return EXIT_FAILURE;
		}

		ShowWindow(_windowHandle, SW_SHOW);

		return EXIT_SUCCESS;
	}
}
