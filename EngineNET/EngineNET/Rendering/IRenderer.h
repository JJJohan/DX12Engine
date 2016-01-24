#pragma once

#include "../stdafx.h"

namespace Engine
{
	class IRenderer;
}

namespace EngineNET
{
	ref class Colour;
	ref class String;

	public ref class IRenderer abstract
	{
	public:
		virtual bool InitWindow(int width, int height, bool windowed);
		virtual void SetClearColour(Colour^ colour);
		void SetVsync(bool enabled);
		void SetWindowTitle(String^ title);
		String^ GetDeviceName();
		int GetDeviceMemoryTotal();
		int GetDeviceMemoryFree();
		String^ GetMaxFeatureLevel();
		int ScreenWidth();
		int ScreenHeight();

	protected:
		Engine::IRenderer* _pIRenderer;
	};
}