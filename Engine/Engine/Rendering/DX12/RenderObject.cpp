#include "RenderObject.h"
#include "VertexBufferInstance.h"
#include "IndexBufferInstance.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "Material.h"
#include "GBuffer.h"

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

	RenderObject::RenderObject(std::string name)
		: _pVertexBuffer(nullptr)
		, _pIndexBuffer(nullptr)
		, _pMaterial(nullptr)
		, _name(name)
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
		if (_pVertexBuffer == nullptr)
		{
			return;
		}

		Camera::Main()->ApplyTransform(_pCbuffer, Transform);
		_pVertexBuffer->SetBufferIndex(_pCbuffer->GetIndex());

		_pVertexBuffer->Unbind();
		_pCbuffer->Unbind();
		if (_pIndexBuffer != nullptr)
		{
			_pIndexBuffer->Unbind();
		}
	}

	void RenderObject::Draw()
	{
		if (_pVertexBuffer == nullptr)
		{
			return;
		}

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

