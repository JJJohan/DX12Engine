#include "../../../stdafx.h"

#include "IndexBuffer.h"
#include "Engine/Rendering/DX12/IndexBuffer.h"
#include <vector>

namespace EngineNET
{
	IndexBufferInstance::IndexBufferInstance()
	{
		_pIndexBufferInstance = new Engine::IndexBufferInstance();
	}

	IndexBufferInstance::~IndexBufferInstance()
	{
		this->!IndexBufferInstance();
	}

	IndexBufferInstance::!IndexBufferInstance()
	{
		delete _pIndexBufferInstance;
	}

	void IndexBufferInstance::SetIndices(System::Collections::Generic::List<int>^ indices)
	{
		int count = indices->Count;
		std::vector<int> vec(count);		
		for (int i = 0; i < count; ++i)
		{
			vec.push_back(indices[i]);
		}

		_pIndexBufferInstance->SetIndices(vec);
	}

	System::Collections::Generic::List<int>^ IndexBufferInstance::GetIndices()
	{
		std::vector<int> vec = _pIndexBufferInstance->GetIndices();
		int count = int(vec.size());
		System::Collections::Generic::List<int>^ indices = gcnew System::Collections::Generic::List<int>(count);
		for (int i = 0; i < count; ++i)
		{
			indices->Add(vec[0]);
		}

		return indices;
	}
}