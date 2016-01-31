#include "RenderObject.h"
#include "Material.h"
#include "../../Data/String.h"
#include "../../Data/Transform.h"
#include "Buffers/IndexBufferInstance.h"
#include "Buffers/VertexBufferInstance.h"
#include "Engine/Rendering/DX12/RenderObject.h"

namespace EngineNET
{
	RenderObject::RenderObject()
	{
		_pRenderObject = new Engine::RenderObject();
		Transform = gcnew EngineNET::Transform(&_pRenderObject->Transform);
	}

	RenderObject::RenderObject(Engine::RenderObject* renderObject)
	{
		_pRenderObject = renderObject;
		Transform = gcnew EngineNET::Transform(&_pRenderObject->Transform);
	}

	RenderObject::RenderObject(System::String^ name)
	{
		_pRenderObject = new Engine::RenderObject(String(name));
		Transform = gcnew EngineNET::Transform(&_pRenderObject->Transform);
	}

	RenderObject::~RenderObject()
	{
		this->!RenderObject();
	}

	RenderObject::!RenderObject()
	{
		delete _pRenderObject;
	}

	RenderObject::operator Engine::RenderObject*()
	{
		return _pRenderObject;
	}

	void RenderObject::SetVertexBuffer(VertexBufferInstance^ vertexBuffer)
	{
		_pRenderObject->SetVertexBuffer(vertexBuffer);
	}

	VertexBufferInstance^ RenderObject::GetVertexBuffer()
	{
		return gcnew VertexBufferInstance(_pRenderObject->GetVertexBuffer());
	}

	void RenderObject::SetIndexBuffer(IndexBufferInstance^ indexBuffer)
	{
		_pRenderObject->SetIndexBuffer(indexBuffer);
	}

	IndexBufferInstance^ RenderObject::GetIndexBuffer()
	{
		return gcnew IndexBufferInstance(_pRenderObject->GetIndexBuffer());
	}

	void RenderObject::SetMaterial(Material^ material)
	{
		_pRenderObject->SetMaterial(material);
	}

	Material^ RenderObject::GetMaterial()
	{
		return gcnew Material(_pRenderObject->GetMaterial());
	}

	void RenderObject::Draw()
	{
		_pRenderObject->Draw();
	}
}