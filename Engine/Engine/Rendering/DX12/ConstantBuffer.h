#pragma once

#define CBUFFER_SLOT_SIZE 64

#include <map>
#include "BufferBucket.h"
#include "BufferInstance.h"

namespace DirectX
{
	struct XMFLOAT4;
	struct XMFLOAT4X4;
}

struct ID3D12GraphicsCommandList;

namespace Engine
{
	class ConstantBuffer : public BufferBucket
	{
	public:
		ConstantBuffer();
		void Build() override;
		void Bind(ID3D12GraphicsCommandList* commandList) override;

	private:
		ID3D12DescriptorHeap* _pDescriptor;

		friend class ConstantBufferInstance;
	};

	class ConstantBufferInstance : public BufferInstance
	{
	public:
		~ConstantBufferInstance();

		void SetFloat(std::string name, float value);
		void SetInt(std::string name, int value);
		void SetVector(std::string name, const DirectX::XMFLOAT4& value);
		void SetMatrix(std::string name, const DirectX::XMFLOAT4X4& value);
		const char* GetData() const;
		size_t GetSize() const override;
		int GetIndex() const;

	private:
		ConstantBufferInstance(ID3D12DescriptorHeap* descriptorHeap);

		void AssignBuffer();

		struct DataItem
		{
			void* Data;
			size_t Size;
		};

		int _index;
		size_t _slotUsage;
		std::map<std::string, DataItem> _cbuffer;
		ID3D12DescriptorHeap* _pDescriptor;

		friend class ConstantBuffer;
		friend class ResourceFactory;
	};
}

