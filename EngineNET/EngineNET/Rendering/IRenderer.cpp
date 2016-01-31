#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "IRenderer.h"
#include "../Data/String.h"
#include "../Data/Colour.h"
#include "Engine/Rendering/IRenderer.h"

namespace EngineNET
{
	void IRenderer::SetClearColour(Colour^ colour)
	{
		_pIRenderer->SetClearColour(*(Engine::Colour*)colour);
	}

	void IRenderer::SetVsync(bool enabled)
	{
		_pIRenderer->SetVsync(enabled);
	}

	void IRenderer::SetWindowTitle(System::String^ title)
	{
		_pIRenderer->SetWindowTitle(String::Sys2Std(title));
	}

	System::String^ IRenderer::GetDeviceName()
	{
		return String::Std2Sys(_pIRenderer->GetDeviceName());
	}

	int IRenderer::GetDeviceMemoryTotal()
	{
		return int(_pIRenderer->GetDeviceMemoryTotal());
	}

	int IRenderer::GetDeviceMemoryFree()
	{
		return int(_pIRenderer->GetDeviceMemoryFree());
	}

	System::String^ IRenderer::GetMaxFeatureLevel()
	{
		return String::Std2Sys(_pIRenderer->GetMaxFeatureLevel());
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