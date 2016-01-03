#include "TextureFactory.h"
#include "../Rendering/DX12/Texture.h"

namespace Engine
{
	Texture* TextureFactory::CreateTexture(int width, int height)
	{
		Texture* texture = new Texture();

		texture->_pDevice = _pRenderer->_device.Get();
		texture->_pSrvHeap = _pRenderer->_srvHeap.Get();
		texture->_width = width;
		texture->_height = height;

		return texture;
	}
}

