#pragma once

struct ID3D12GraphicsCommandList;
struct ID3D12Device;

namespace Engine
{
	class BufferBucket;

	class BufferInstance
	{
	public:
		BufferInstance();
		virtual ~BufferInstance();

		void Bind(ID3D12GraphicsCommandList* commandList) const;
		size_t GetOffset() const;
		virtual size_t GetSize() const = 0;

	protected:
		BufferBucket* _pBuffer;
		void SetOffset(size_t offset);

	private:
		ID3D12Device* _pDevice;
		size_t _offset;

		friend class BufferBucket;
		friend class ResourceFactory;
	};
}

