#pragma once

#include "../../stdafx.h"
#include "../IRenderer.h"

namespace Engine
{
	class DX12Renderer;
}

namespace EngineNET
{
	public ref class DX12Renderer : public IRenderer
	{
	public:
		DX12Renderer(Engine::IRenderer* renderer);
		~DX12Renderer();
		!DX12Renderer();
	};
}