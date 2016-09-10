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
		void SetRootSlot(int value);
		size_t GetOffset() const;
		virtual size_t Count() const = 0;
		virtual size_t GetSize() const = 0;
		void Unbind() const;
		bool Bound() const;

	protected:
		BufferBucket* _pBuffer;
		void SetOffset(size_t offset);

	private:
		ID3D12Device* _pDevice;
		size_t _offset;
		int _rootSlot;

		friend class BufferBucket;
		friend class ResourceFactory;
	};
}

