#pragma once

#include "../stdafx.h"

namespace Engine
{
	class Renderer;
}

namespace EngineNET
{
	ref class IRenderer;

	public ref class Renderer
	{
	public:
		static IRenderer^ CreateRenderer();
	};
}