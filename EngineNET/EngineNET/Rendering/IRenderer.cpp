#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "IRenderer.h"
#include "../Data/String.h"
#include "../Data/Colour.h"
#include "Engine/Rendering/IRenderer.h"

namespace EngineNET
{
	bool IRenderer::InitWindow(int width, int height, bool windowed)
	{
		return _pIRenderer->InitWindow(width, height, windowed);
	}

	void IRenderer::SetClearColour(Colour^ colour)
	{
		_pIRenderer->SetClearColour(*(Engine::Colour*)colour);
	}

	void IRenderer::SetVsync(bool enabled)
	{
		_pIRenderer->SetVsync(enabled);
	}

	void IRenderer::SetWindowTitle(String^ title)
	{
		_pIRenderer->SetWindowTitle(title->CStr());
	}

	String^ IRenderer::GetDeviceName()
	{
		return gcnew String(_pIRenderer->GetDeviceName());
	}

	int IRenderer::GetDeviceMemoryTotal()
	{
		return int(_pIRenderer->GetDeviceMemoryTotal());
	}

	int IRenderer::GetDeviceMemoryFree()
	{
		return int(_pIRenderer->GetDeviceMemoryFree());
	}

	String^ IRenderer::GetMaxFeatureLevel()
	{
		return gcnew String(_pIRenderer->GetMaxFeatureLevel());
	}

	int IRenderer::ScreenWidth()
	{
		return _pIRenderer->ScreenWidth();
	}

	int IRenderer::ScreenHeight()
	{
		return _pIRenderer->ScreenHeight();
	}
}