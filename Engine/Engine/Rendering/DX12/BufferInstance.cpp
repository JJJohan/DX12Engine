#include "BufferInstance.h"
#include "BufferBucket.h"

namespace Engine
{
	BufferInstance::BufferInstance()
		: _pBuffer(nullptr)
		, _pDevice(nullptr)
		, _offset(0)
	{
	}

	void BufferInstance::SetOffset(size_t offset)
	{
		_offset = offset;
	}

	BufferInstance::~BufferInstance()
	{
		if (_pBuffer != nullptr)
		{
			_pBuffer->ReleaseInstance(this);
		}
	}

	void BufferInstance::Bind(ID3D12GraphicsCommandList* commandList) const
	{
		_pBuffer->Bind(commandList);
	}

	size_t BufferInstance::GetOffset() const
	{
		return _offset;
	}

	void BufferInstance::Unbind() const
	{
		_pBuffer->Unbind();
	}
}

