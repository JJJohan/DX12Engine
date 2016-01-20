#include "IRenderer.h"
#include "Renderer.h"

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif
#ifndef HID_USAGE_GENERIC_KEYBOARD
#define HID_USAGE_GENERIC_KEYBOARD        ((USHORT) 0x06)
#endif

namespace Engine
{
	HWND IRenderer::_windowClosed = nullptr;
	bool IRenderer::_maximized = false;

	IRenderer::IRenderer()
		: _screenWidth(0)
		  , _screenHeight(0)
		  , _aspectRatio(1.0f)
		  , _windowed(false)
		  , _windowHandle(nullptr)
		  , _windowInstance(HINSTANCE(GetModuleHandle(nullptr)))
		  , _deviceMemoryTotal(0)
		  , _deviceMemoryFree(0)
		  , _vsync(true)
		  , _renderFinished(true)
	{
		_windowClosed = nullptr;
		IRenderer::SetClearColour(Colour::Blue);
	}

	IRenderer::~IRenderer()
	{
	}

	bool IRenderer::RenderFinished() const
	{
		return _renderFinished;
	}

	void IRenderer::SetClearColour(const Colour& colour)
	{
		_clearColour[0] = colour.GetRed();
		_clearColour[1] = colour.GetGreen();
		_clearColour[2] = colour.GetBlue();
		_clearColour[3] = colour.GetAlpha();
	}

	bool IRenderer::WindowRender() const
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

		case WM_SIZE:
			Renderer::GetRenderer()->_screenWidth = LOWORD(lParam);
			Renderer::GetRenderer()->_screenHeight = HIWORD(lParam);
			if (wParam == SIZE_MAXIMIZED || _maximized)
			{
				_maximized = (wParam == SIZE_MAXIMIZED);
				Renderer::GetRenderer()->Resize(LOWORD(lParam), HIWORD(lParam));
			}
			break;

		case WM_EXITSIZEMOVE:
			Renderer::GetRenderer()->Resize(LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_INPUT:
			{
				UINT dwSize = 0;

				GetRawInputData(HRAWINPUT(lParam), RID_INPUT, nullptr, &dwSize,
				                sizeof(RAWINPUTHEADER));
				std::unique_ptr<BYTE[]> lpb(new BYTE[size_t(dwSize)], std::default_delete<BYTE[]>());

				if (GetRawInputData(HRAWINPUT(lParam), RID_INPUT, lpb.get(), &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
				{
					OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));
				}

				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb.get());

				if (raw->header.dwType == RIM_TYPEKEYBOARD)
				{
					if (raw->data.keyboard.Flags == RI_KEY_BREAK)
					{
						Input::KeyUpEvent(raw->data.keyboard.VKey);
					}
					else if (raw->data.keyboard.Flags == RI_KEY_MAKE)
					{
						Input::KeyDownEvent(raw->data.keyboard.VKey);
					}
				}
				else if (raw->header.dwType == RIM_TYPEMOUSE)
				{
					// Check if mouse has moved.
					if (raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
					{
						Input::MouseMoveEvent(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
					}

					// Check if mouse buttons were clicked.
					switch (raw->data.mouse.usButtonFlags)
					{
					case RI_MOUSE_LEFT_BUTTON_DOWN:
						Input::MouseButtonDownEvent(0);
						break;
					case RI_MOUSE_LEFT_BUTTON_UP:
						Input::MouseButtonUpEvent(0);
						break;
					case RI_MOUSE_MIDDLE_BUTTON_DOWN:
						Input::MouseButtonDownEvent(1);
						break;
					case RI_MOUSE_MIDDLE_BUTTON_UP:
						Input::MouseButtonUpEvent(2);
						break;
					case RI_MOUSE_RIGHT_BUTTON_DOWN:
						Input::MouseButtonDownEvent(2);
						break;
					case RI_MOUSE_RIGHT_BUTTON_UP:
						Input::MouseButtonUpEvent(2);
						break;
					}
				}

				break;
			}

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}

		return EXIT_SUCCESS;
	}

	void IRenderer::Resize(float width, float height)
	{
	}

	void IRenderer::AssignCreateMethod(const std::function<void()>& createMethod)
	{
		_createMethod = createMethod;
	}

	void IRenderer::AssignDrawLoop(const std::function<void()>& drawLoop)
	{
		_drawLoop = drawLoop;
	}

	void IRenderer::SetVsync(bool enabled)
	{
		_vsync = enabled;
	}

	std::string IRenderer::GetDeviceName() const
	{
		return _deviceName;
	}

	size_t IRenderer::GetDeviceMemoryTotal() const
	{
		return _deviceMemoryTotal;
	}

	size_t IRenderer::GetDeviceMemoryFree() const
	{
		return _deviceMemoryFree;
	}

	std::string IRenderer::GetMaxFeatureLevel() const
	{
		return _featureLevel;
	}

	void IRenderer::SetWindowTitle(const std::string& title) const
	{
		if (_windowHandle != nullptr)
		{
			std::wstring wide = std::wstring(title.begin(), title.end());
			SetWindowText(_windowHandle, wide.c_str());
		}
	}

	HWND IRenderer::GetWindowHandle() const
	{
		return _windowHandle;
	}

	int IRenderer::ScreenHeight() const
	{
		return _screenHeight;
	}


	int IRenderer::ScreenWidth() const
	{
		return _screenWidth;
	}

	bool IRenderer::RegisterInstance() const
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
		wcx.lpszClassName = L"EngineProcess"; // name of window class 
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
		_windowHandle = CreateWindow(L"EngineProcess", L"Engine Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			_screenWidth, _screenHeight, nullptr, nullptr, _windowInstance, nullptr);

		RAWINPUTDEVICE rid[2];
		rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC; // 0x01
		rid[0].usUsage = HID_USAGE_GENERIC_MOUSE; // 0x02
		rid[0].dwFlags = RIDEV_INPUTSINK;
		rid[0].hwndTarget = _windowHandle;

		rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC; // 0x01
		rid[1].usUsage = HID_USAGE_GENERIC_KEYBOARD; // 0x06
		rid[1].dwFlags = RIDEV_INPUTSINK;
		rid[1].hwndTarget = _windowHandle;

		if (!RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE)))
		{
			return EXIT_FAILURE;
		}

		if (!_windowHandle)
		{
			return EXIT_FAILURE;
		}

		ShowWindow(_windowHandle, SW_SHOW);

		return EXIT_SUCCESS;
	}
}

