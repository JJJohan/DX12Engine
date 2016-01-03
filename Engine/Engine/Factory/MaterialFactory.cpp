#include "MaterialFactory.h"
#include "../Rendering/DX12/Material.h"

namespace Engine
{
	Material* MaterialFactory::CreateMaterial()
	{
		Material* material = new Material();

		material->_pDevice = _pRenderer->_device.Get();
		material->_pRootSignature = _pRenderer->_rootSignature.Get();

		return material;
	}
}

