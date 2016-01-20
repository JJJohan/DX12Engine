#pragma once

#include <functional>

namespace Engine
{
	class Colour;

	class IRenderer
	{
	public:
		IRenderer();
		virtual ~IRenderer();

		ENGINE_API virtual bool InitWindow(int width, int height, bool windowed);
		virtual bool Render() = 0;
		virtual bool Update() = 0;
		bool WindowRender() const;
		ENGINE_API virtual void SetClearColour(const Colour& colour);
		ENGINE_API void SetVsync(bool enabled);
		ENGINE_API void SetWindowTitle(const std::string& title) const;
		ENGINE_API std::string GetDeviceName() const;
		ENGINE_API size_t GetDeviceMemoryTotal() const;
		ENGINE_API size_t GetDeviceMemoryFree() const;
		ENGINE_API std::string GetMaxFeatureLevel() const;
		ENGINE_API HWND GetWindowHandle() const;
		bool RenderFinished() const;
		void AssignDrawLoop(const std::function<void()>& drawLoop);
		void AssignCreateMethod(const std::function<void()>& createMethod);
		ENGINE_API int ScreenWidth() const;
		ENGINE_API int ScreenHeight() const;

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

