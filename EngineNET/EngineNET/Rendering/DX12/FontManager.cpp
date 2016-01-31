#include "Font.h"
#include "FontManager.h"
#include "../../Data/String.h"
#include "Engine/Rendering/DX12/FontManager.h"

namespace EngineNET
{
	Font^ FontManager::LoadFont(System::String^ fontName, System::String^ textureFile, System::String^ fontFile)
	{
		return gcnew Font(Engine::FontManager::LoadFont(String(fontName), String(textureFile), String(fontFile)));
	}

	Font^ FontManager::GetFont(System::String^ fontName)
	{
		return gcnew Font(const_cast<Engine::Font*>(Engine::FontManager::GetFont(String(fontName))));
	}
}