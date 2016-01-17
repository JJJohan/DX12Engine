#include "RenderObject.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Material.h"
#include "../../Factory/ResourceFactory.h"
#include "Camera.h"
#include "ConstantBuffer.h"

namespace Engine
{
	std::unordered_set<RenderObject*> RenderObject::_renderObjects;

	RenderObject::RenderObject()
		: _pVertexBuffer(nullptr)
		  , _pIndexBuffer(nullptr)
		  , _pMaterial(nullptr)
	{
		_renderObjects.insert(this);
		_pCbuffer = ResourceFactory::CreateConstantBuffer();
	}

	RenderObject::~RenderObject()
	{
		auto it = _renderObjects.find(this);
		if (it != _renderObjects.end())
		{
			_renderObjects.erase(it);
		}

		delete _pCbuffer;
		delete _pVertexBuffer;
		delete _pIndexBuffer;
	}

	void RenderObject::SetVertexBuffer(VertexBufferInstance* vertexBuffer)
	{
		_pVertexBuffer = vertexBuffer;
	}

	VertexBufferInstance* RenderObject::GetVertexBuffer() const
	{
		return _pVertexBuffer;
	}

	void RenderObject::SetIndexBuffer(IndexBufferInstance* indexBuffer)
	{
		_pIndexBuffer = indexBuffer;
	}

	IndexBufferInstance* RenderObject::GetIndexBuffer() const
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

	void RenderObject::Update()
	{
		Camera::Main()->ApplyTransform(_pCbuffer, Transform);
		_pVertexBuffer->SetBufferIndex(_pCbuffer->GetIndex());
	}

	void RenderObject::Draw()
	{
		ID3D12GraphicsCommandList* commandList = static_cast<ID3D12GraphicsCommandList*>(ResourceFactory::GetCommandList());
		_pMaterial->Bind(commandList);
		_pVertexBuffer->Bind(commandList);
		_pCbuffer->Bind(commandList);

		if (_pIndexBuffer != nullptr)
		{
			_pIndexBuffer->Bind(commandList);
			commandList->DrawIndexedInstanced(UINT(_pIndexBuffer->Count()), 1, UINT(_pIndexBuffer->GetOffset()), UINT(_pVertexBuffer->GetOffset()), 0);
		}
		else
		{
			commandList->DrawInstanced(UINT(_pVertexBuffer->Count()), 1, UINT(_pVertexBuffer->GetOffset()), 0);
		}
	}
}

