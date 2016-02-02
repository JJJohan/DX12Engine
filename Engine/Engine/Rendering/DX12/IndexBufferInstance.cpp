#include "IndexBuffer.h"
#include "IndexBufferInstance.h"

namespace Engine
{
	IndexBufferInstance::IndexBufferInstance() { }

	void IndexBufferInstance::SetIndices(std::vector<int> indices)
	{
		_indices = indices;

		if (_pBuffer == nullptr)
		{
			_pBuffer = BufferBucket::PrepareBuffer<IndexBuffer>(this);
		}
		else
		{
			IndexBuffer* buffer = static_cast<IndexBuffer*>(_pBuffer);
			buffer->RequestBuild();
		}
	}

	std::vector<int> IndexBufferInstance::GetIndices() const
	{
		return _indices;
	}

	size_t IndexBufferInstance::Count() const
	{
		return _indices.size();
	}

	size_t IndexBufferInstance::GetSize() const
	{
		return Count() * sizeof(int);
	}
}

