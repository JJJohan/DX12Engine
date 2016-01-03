#include "RenderObjectFactory.h"
#include "../Rendering/DX12/RenderObject.h"

namespace Engine
{
	RenderObject* RenderObjectFactory::CreateRenderObject()
	{
		RenderObject* renderObject = new RenderObject();

		renderObject->_device = _pRenderer->_device;

		return renderObject;
	}
}

