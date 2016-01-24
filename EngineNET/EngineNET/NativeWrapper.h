#pragma once

#include "NativeWrapper.h"

namespace Engine
{
	template <typename T>
	public ref class NativeWrapper
	{
	public:
		NativeWrapper()
		{
			_pNativeClass = new T();
		}

		~NativeWrapper()
		{
			this->!NativeWrapper();
		}

		!NativeWrapper()
		{
			delete _pNativeClass;
		}

	private:
		T* _pNativeClass;
	};
}