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

		ENGINE_API size_t Count() const override;
		size_t GetSize() const override;

	private:
		std::vector<int> _indices;

		friend class IndexBuffer;
	};
}