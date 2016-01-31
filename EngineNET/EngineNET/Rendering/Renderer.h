#pragma once

using namespace System;

namespace Engine
{
	class IRenderer;
	class Renderer;
}

namespace EngineNET
{
	ref class IRenderer;

	public ref class Renderer
	{
	public:
		static IRenderer^ GetRenderer();
		static IRenderer^ CreateRenderer();
		static void ProcessWindowMessage(IntPtr^ hwnd, int msg, IntPtr^ wParam, IntPtr^ lParam);		

	private:
		static IRenderer^ _renderer;
	};
}