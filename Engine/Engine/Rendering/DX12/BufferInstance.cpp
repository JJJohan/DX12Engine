#include "BufferInstance.h"
#include "BufferBucket.h"

namespace Engine
{
	BufferInstance::BufferInstance()
		: _pBuffer(nullptr)
		, _pDevice(nullptr)
		, _offset(0)
	{ }

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
		if (_pBuffer != nullptr)
		{
			_pBuffer->Bind(commandList);
		}
	}

	void BufferInstance::SetRootSlot(int value)
	{
		_rootSlot = value;
	}

	size_t BufferInstance::GetOffset() const
	{
		return _offset;
	}

	void BufferInstance::Unbind() const
	{
		_pBuffer->Unbind();
	}

	bool BufferInstance::Bound() const
	{
		if (_pBuffer == nullptr)
		{
			return false;
		}

		return _pBuffer->Bound();
	}
}