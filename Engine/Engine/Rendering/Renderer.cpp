#include "Renderer.h"
#include "DX12\DX12Renderer.h"

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

	Renderer::Renderer()
	{
	}

	Renderer::~Renderer()
	{
		_renderer.reset();
	}
}
