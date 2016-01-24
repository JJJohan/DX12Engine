#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Renderer.h"
#include "IRenderer.h"
#include "DX12\DX12Renderer.h"
#include "Engine/Rendering/Renderer.h"

namespace EngineNET
{
	IRenderer^ Renderer::CreateRenderer()
	{
		Engine::IRenderer* renderer = Engine::Renderer::CreateRenderer(Engine::RenderAPI::Direct3D12);
		DX12Renderer^ dx12 = gcnew DX12Renderer(renderer);
		return dx12;
	}
}