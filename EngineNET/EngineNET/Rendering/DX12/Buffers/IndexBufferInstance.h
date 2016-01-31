#pragma once

#include "../../../stdafx.h"

namespace Engine
{
	class IndexBufferInstance;
}

namespace EngineNET
{
	public ref class IndexBufferInstance
	{
	public:
		IndexBufferInstance();
		IndexBufferInstance(Engine::IndexBufferInstance* instance);
		operator Engine::IndexBufferInstance*();

		void SetIndices(System::Collections::Generic::List<int>^ indices);
		System::Collections::Generic::List<int>^ GetIndices();
		int Count();

	private:
		Engine::IndexBufferInstance* _pIndexBufferInstance;
	};
}