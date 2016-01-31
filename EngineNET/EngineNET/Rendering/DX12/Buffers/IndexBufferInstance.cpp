#include "../../../stdafx.h"

#include "IndexBufferInstance.h"
#include "Engine/Rendering/DX12/IndexBufferInstance.h"
#include <vector>

namespace EngineNET
{
	IndexBufferInstance::IndexBufferInstance()
	{
		_pIndexBufferInstance = new Engine::IndexBufferInstance();
	}

	IndexBufferInstance::IndexBufferInstance(Engine::IndexBufferInstance* instance)
	{
		_pIndexBufferInstance = instance;
	}

	IndexBufferInstance::operator Engine::IndexBufferInstance*()
	{
		return _pIndexBufferInstance;
	}

	void IndexBufferInstance::SetIndices(System::Collections::Generic::List<int>^ indices)
	{
		int count = indices->Count;
		std::vector<int> vec(count);		
		for (int i = 0; i < count; ++i)
		{
			vec[i] = indices[i];
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
			indices->Add(vec[i]);
		}

		return indices;
	}

	int IndexBufferInstance::Count()
	{
		return int(_pIndexBufferInstance->Count());
	}
}