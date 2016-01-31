#include "../../stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Engine/Rendering/IRenderer.h"
#include "DX12Renderer.h"
#include "../IRenderer.h"

namespace EngineNET
{
	DX12Renderer::DX12Renderer(Engine::IRenderer* renderer)
	{
		_pIRenderer = renderer;
	}
}