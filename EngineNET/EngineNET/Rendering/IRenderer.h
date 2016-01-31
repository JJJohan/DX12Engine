#pragma once

namespace Engine
{
	class IRenderer;
}

namespace EngineNET
{
	ref class Colour;

	public ref class IRenderer abstract
	{
	public:
		virtual void SetClearColour(Colour^ colour);
		void SetVsync(bool enabled);
		void SetWindowTitle(System::String^ title);
		System::String^ GetDeviceName();
		int GetDeviceMemoryTotal();
		int GetDeviceMemoryFree();
		System::String^ GetMaxFeatureLevel();
		int ScreenWidth();
		int ScreenHeight();

	protected:
		Engine::IRenderer* _pIRenderer;
	};
}