#include "VertexBuffer.h"
#include "../../Utils/Logging.h"
#include "HeapManager.h"

namespace Engine
{
	std::unordered_set<VertexBuffer*> VertexBuffer::_vertexBuffers;


	VertexBuffer::VertexBuffer()
		: _totalSize(0)
		, _vertexBufferView()
	{
	}

	VertexBufferInstance::VertexBufferInstance(VertexType vertexType)
		: _pDevice(nullptr)
		, _pBuffer(nullptr)
		, _offset(0)
		, _size(0)
		, _vertexSize(0)
	{
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;

		switch (vertexType)
		{
		case VERTEX_POS_COL:
			inputElementDescs =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			_inputLayout = inputElementDescs;
			_vertexSize = sizeof(VertexPosCol);
			return;

		case VERTEX_POS_COL_UV:
			inputElementDescs =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			_inputLayout = inputElementDescs;
			_vertexSize = sizeof(VertexPosColUv);
			return;

		case VERTEX_POS_UV:
			inputElementDescs =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			_inputLayout = inputElementDescs;
			_vertexSize = sizeof(VertexPosUv);
			return;

		default:
			Logging::LogError("Unknown vertex layout.");
			break;
		}
	}

	void VertexBuffer::Bind(ID3D12GraphicsCommandList* commandList) const
	{
		commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
	}

	void VertexBufferInstance::Bind(ID3D12GraphicsCommandList* commandList) const
	{
		_pBuffer->Bind(commandList);
	}

	VertexBuffer::~VertexBuffer()
	{
		auto it = _vertexBuffers.find(this);
		if (it != _vertexBuffers.end())
		{
			_vertexBuffers.erase(it);
		}
	}

	VertexBufferInstance::~VertexBufferInstance()
	{
		if (_pBuffer != nullptr)
		{
			auto it = _pBuffer->_instances.find(this);
			if (it != _pBuffer->_instances.end())
			{
				_pBuffer->_instances.erase(it);
			}
		}

		for (auto it = _vertices.begin(); it != _vertices.end(); ++it)
		{
			delete *it;
		}
	}

	const std::vector<D3D12_INPUT_ELEMENT_DESC>& VertexBufferInstance::GetInputLayout() const
	{
		return _inputLayout;
	}

	size_t VertexBufferInstance::VertexSize() const
	{
		return _vertexSize;
	}

	size_t VertexBufferInstance::Count() const
	{
		return _vertices.size();
	}

	void VertexBufferInstance::SetVertices(std::vector<void*> vertices)
	{
		if (_pBuffer == nullptr)
		{
			_pBuffer = VertexBuffer::PrepareBuffer(this, _pDevice);
		}

		_vertices = vertices;
		_pBuffer->RequestBuild();
	}

	std::vector<void*> VertexBufferInstance::GetVertices() const
	{
		return _vertices;
	}

	void VertexBuffer::RequestBuild()
	{
		HeapTask(std::bind(&VertexBuffer::Build, this));
	}

	void VertexBuffer::Build()
	{
		// Calculate total size of buffer.
		size_t size = 0;
		for (auto it = _instances.begin(); it != _instances.end(); ++it)
		{
			VertexBufferInstance* instance = *it;
			size += instance->VertexSize() * instance->Count();
		}

		if (size >= 65536)
		{
			Logging::LogError("Vertex buffer exceeded maximum size of 64KB.");
			return;
		}

		// Perform memory copy.
		size_t offset = 0;
		//std::unique_ptr<char> memory(new char[size]);
		char* memory = new char[size];
		for (auto it = _instances.begin(); it != _instances.end(); ++it)
		{
			VertexBufferInstance* instance = *it;
			std::vector<void*> vertices = instance->GetVertices();
			size_t copySize = instance->VertexSize() * instance->Count();

			memcpy(memory + offset, &vertices[0], copySize);
			offset += copySize;
		}

		_heapSize = size_t(offset);
		PrepareHeapResource();

		HeapManager::Upload(this, memory, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		// Initialize the vertex buffer view.
		_vertexBufferView.BufferLocation = _pResource->GetGPUVirtualAddress();
		_vertexBufferView.StrideInBytes = sizeof(VertexPosUv);
		_vertexBufferView.SizeInBytes = UINT(_heapSize);
	}

	VertexBuffer* VertexBuffer::PrepareBuffer(VertexBufferInstance* instance, ID3D12Device* device)
	{
		for (auto it = _vertexBuffers.begin(); it != _vertexBuffers.end(); ++it)
		{
			if ((*it)->_totalSize < 60000) // +10% free space
			{
				(*it)->_instances.insert(instance);
				return *it;
			}
		}

		VertexBuffer* vertexBuffer = new VertexBuffer();
		vertexBuffer->_pDevice = device;
		vertexBuffer->_instances.insert(instance);
		_vertexBuffers.insert(vertexBuffer);
		return vertexBuffer;
	}
}

