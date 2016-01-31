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
		_renderer = gcnew DX12Renderer(renderer);
		return _renderer;
	}

	void Renderer::ProcessWindowMessage(IntPtr^ hwnd, int msg, IntPtr^ wParam, IntPtr^ lParam)
	{
		Engine::Renderer::ProcessWindowMessage((HWND)hwnd->ToPointer(), (UINT)msg, (WPARAM)wParam->ToPointer(), (LPARAM)lParam->ToPointer());
	}

	IRenderer^ Renderer::GetRenderer()
	{
		return _renderer;
	}
}