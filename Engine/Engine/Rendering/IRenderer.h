#pragma once
#include <windows.h>
#include "..\Data\Colour.h"
#include <string>
#include <functional>

namespace Engine
{
	class IRenderer
	{
	public:
		IRenderer();
		virtual ~IRenderer();

		virtual bool InitWindow(int width, int height, bool windowed);
		virtual bool Render() = 0;
		virtual bool Update() = 0;
		bool WindowRender() const;
		virtual void SetClearColour(const Colour& colour);
		void SetVsync(bool enabled);
		void SetWindowTitle(const std::string& title) const;
		std::string GetDeviceName() const;
		size_t GetDeviceMemoryTotal() const;
		size_t GetDeviceMemoryFree() const;
		std::string GetMaxFeatureLevel() const;
		HWND GetWindowHandle() const;
		bool RenderFinished() const;
		void AssignDrawLoop(const std::function<void()>& drawLoop);
		void AssignCreateMethod(const std::function<void()>& createMethod);
		int GetFPSLimit() const;
		int ScreenWidth() const;
		int ScreenHeight() const;

	protected:
		int _screenWidth;
		int _screenHeight;
		float _aspectRatio;
		bool _windowed;
		HWND _windowHandle;
		HINSTANCE _windowInstance;
		float _clearColour[4];
		std::string _deviceName;
		std::string _featureLevel;
		size_t _deviceMemoryTotal;
		size_t _deviceMemoryFree;
		bool _vsync;
		volatile bool _renderFinished;

		virtual void Resize(float width, float height);
		std::function<void()> _createMethod;
		std::function<void()> _drawLoop;

	private:
		bool RegisterInstance() const;
		bool RegisterWindow();

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static HWND _windowClosed;
		static bool _maximized;
	};
}

