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

		void SetFloat(std::string name, float value);
		void SetInt(std::string name, int value);
		void SetVector(std::string name, const DirectX::XMFLOAT4& value);
		void SetMatrix(std::string name, const DirectX::XMFLOAT4X4& value);
		std::vector<DataItem> GetData() const;
		size_t GetSize() const override;
		int GetIndex() const;
		void SetIndex(int index);
		void SetVertexBuffer(VertexBufferInstance* vertexBuffer);

	private:
		ConstantBufferInstance(ID3D12DescriptorHeap* descriptorHeap);

		void AssignBuffer();

		int _index;
		size_t _slotUsage;
		std::map<std::string, DataItem> _cbuffer;
		ID3D12DescriptorHeap* _pDescriptor;
		VertexBufferInstance* _pVertexBuffer;

		friend class ConstantBuffer;
		friend class ResourceFactory;
	};
}

