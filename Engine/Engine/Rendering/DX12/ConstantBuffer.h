#pragma once

#include <map>
#include "HeapResource.h"

namespace DirectX
{
	struct XMFLOAT4;
	struct XMFLOAT4X4;
}

struct ID3D12GraphicsCommandList;

namespace Engine
{
	class ConstantBuffer : public HeapResource
	{
	public:
		~ConstantBuffer();

		void SetFloat(std::string name, float value);
		void SetInt(std::string name, int value);
		void SetVector(std::string name, const DirectX::XMFLOAT4& value);
		void SetMatrix(std::string name, const DirectX::XMFLOAT4X4& value);

		void Bind(ID3D12GraphicsCommandList* commandList) const;

	private:
		ConstantBuffer();
		void UpdateBuffer();

		struct DataItem
		{
			void* Data;
			size_t Size;
		};

		ID3D12DescriptorHeap* _pHeap;
		int _index;
		int _descriptorSize;
		std::map<std::string, DataItem> _cbuffer;
		bool _dirty;
		size_t _bufferSize;


		friend class ResourceFactory;
	};
}

