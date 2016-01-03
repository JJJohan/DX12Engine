#include "Renderer.h"
#include "DX12\DX12Renderer.h"
#include "../Core/Core.h"

namespace Engine
{
	std::unique_ptr<IRenderer> _renderer;

	IRenderer* Renderer::CreateRenderer(RenderAPI renderAPI)
	{
		switch (renderAPI)
		{
		case Direct3D12:
			_renderer = std::make_unique<DX12Renderer>();
			break;
		}

		return _renderer.get();
	}

	void Renderer::DestroyRenderer()
	{
		_renderer.reset();
	}

	bool Renderer::Render()
	{
		return _renderer.get()->Render();
	}

	bool Renderer::Update()
	{
		return _renderer.get()->Update();
	}

	HWND Renderer::GetWindowHandle()
	{
		return _renderer.get()->GetWindowHandle();
	}

	Renderer::Renderer()
	{
	}

	Renderer::~Renderer()
	{
		_renderer.reset();
	}

	IRenderer* Renderer::GetRenderer()
	{
		return _renderer.get();
	}
}

