#include "RenderObject.h"
#include "IndexBuffer.h"
#include "Material.h"
#include "../../Factory/Factory.h"

namespace Engine
{
	RenderObject::RenderObject()
		: _pVertexBuffer(nullptr)
		  , _pIndexBuffer(nullptr)
		  , _pMaterial(nullptr)
	{
	}

	RenderObject::~RenderObject()
	{
		_device.Reset();

		delete _pVertexBuffer;
		delete _pIndexBuffer;
	}

	void RenderObject::SetIndexBuffer(IndexBuffer* indexBuffer)
	{
		_pIndexBuffer = indexBuffer;
	}

	IndexBuffer* RenderObject::GetIndexBuffer() const
	{
		return _pIndexBuffer;
	}

	void RenderObject::SetMaterial(Material* material)
	{
		_pMaterial = material;
	}

	Material* RenderObject::GetMaterial() const
	{
		return _pMaterial;
	}

	void RenderObject::Draw() const
	{
		ID3D12GraphicsCommandList* commandList = static_cast<ID3D12GraphicsCommandList*>(Factory::GetCommandList());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_pMaterial->Bind(commandList);
		_pVertexBuffer->Bind(commandList);
		commandList->DrawInstanced(UINT(_pVertexBuffer->Count()), 1, 0, 0);
	}
}

