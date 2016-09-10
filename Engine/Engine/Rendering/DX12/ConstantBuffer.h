#pragma once

#define CBUFFER_SLOT_SIZE 64

#include "BufferBucket.h"
#include "BufferInstance.h"

namespace DirectX
{
	struct XMFLOAT4;
	struct XMFLOAT4X4;
}

struct ID3D12GraphicsCommandList;
struct ID3D12DescriptorHeap;

namespace Engine
{
	class VertexBufferInstance;

	class ConstantBuffer : public BufferBucket
	{
	public:
		ConstantBuffer();
		~ConstantBuffer();
		void Build() override;
		void Bind(ID3D12GraphicsCommandList* commandList) override;

	private:
		ID3D12DescriptorHeap* _pDescriptor;
		char* _pCopyBuffer;
		size_t _copyBufferSize;

		friend class ConstantBufferInstance;
	};

	class ConstantBufferInstance : public BufferInstance
	{
		struct DataItem
		{
			void* Data;
			size_t Size;
		};

	public:
		ENGINE_API ~ConstantBufferInstance();

		std::vector<DataItem> GetData() const;
		size_t Count() const override;
		size_t GetSize() const override;
		int GetIndex() const;
		void SetIndex(int index);
		void SetVertexBuffer(VertexBufferInstance* vertexBuffer);
		void AssignBuffer();

		template <class T>
		void SetData(std::string name, T value)
		{
			if (_cbuffer.find(name) == _cbuffer.end())
			{
				if (_slotUsage + sizeof(T) > CBUFFER_SLOT_SIZE)
				{
					Logging::LogError("Variable '{0}' has exceeded slot size.", name);
					return;
				}

				_slotUsage += sizeof(T);
				_cbuffer[name] = { new T(value), sizeof(T) };
			}
			else
			{
				*static_cast<T*>(_cbuffer[name].Data) = value;
			}

			AssignBuffer();
		}

	private:
		ConstantBufferInstance(ID3D12DescriptorHeap* descriptorHeap);

		int _index;
		size_t _slotUsage;
		std::map<std::string, DataItem> _cbuffer;
		ID3D12DescriptorHeap* _pDescriptor;
		VertexBufferInstance* _pVertexBuffer;
		int _rootSlot;

		friend class ConstantBuffer;
		friend class ResourceFactory;
	};
}

