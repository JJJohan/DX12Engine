#pragma once

#include "BufferInstance.h"

namespace Engine
{
	class IndexBufferInstance : public BufferInstance
	{
	public:
		ENGINE_API IndexBufferInstance();

		ENGINE_API void SetIndices(std::vector<int> indices);
		ENGINE_API std::vector<int> GetIndices() const;

		size_t Count() const;
		size_t GetSize() const override;

	private:
		std::vector<int> _indices;

		friend class IndexBuffer;
	};
}