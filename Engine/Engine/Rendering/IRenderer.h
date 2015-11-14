#pragma once
#include <windows.h>
#include "..\Data\Colour.h"
#include <string>

namespace Engine
{
	class IRenderer
	{
	public:
		IRenderer();
		virtual ~IRenderer();

		virtual bool InitWindow(int width, int height, bool windowed);
		virtual bool Render();
		virtual bool Update() = 0;
		virtual void SetClearColour(Colour colour);
		void SetVsync(bool enabled);
		void SetWindowTitle(std::string title);

	protected:
		int _screenWidth;
		int _screenHeight;
		float _aspectRatio;
		bool _windowed;
		HWND _windowHandle;
		HINSTANCE _windowInstance;
		float _clearColour[4];
		bool _vsync;

	private:
		bool RegisterInstance();
		bool RegisterWindow();
		
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static HWND _windowClosed;
	};
}
