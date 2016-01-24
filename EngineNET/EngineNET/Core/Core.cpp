#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vcclr.h>

#include "Core.h"
#include "../Data/String.h"
#include "Engine/Core/Core.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace EngineNET
{
	Core::Core()
	{
		_pCore = new Engine::Core();
	}

	Core::~Core()
	{
		this->!Core();
	}

	Core::!Core()
	{
		delete _pCore;
	}

	bool Core::Update()
	{
		return _pCore->Update();
	}

	String^ Core::GetApplicationDirectory()
	{
		return gcnew String(_pCore->GetApplicationDirectory());
	}

	bool Core::Running()
	{
		return _pCore->Running();
	}

	void Core::Exit()
	{
		_pCore->Exit();
	}

	void Core::Destroy()
	{
		_pCore->Destroy();
	}

	void Core::Initialise(int width, int height, bool windowed, StartFunc^ start, UpdateFunc^ update, DrawFunc^ draw, DestroyFunc^ destroy)
	{		
		IntPtr startPtr = Marshal::GetFunctionPointerForDelegate(start);
		IntPtr updatePtr = Marshal::GetFunctionPointerForDelegate(update);
		IntPtr drawPtr = Marshal::GetFunctionPointerForDelegate(draw);
		IntPtr destroyPtr = Marshal::GetFunctionPointerForDelegate(destroy);
		std::function<void(void)> startCallback = static_cast<void(__stdcall *) (void)>(startPtr.ToPointer());
		std::function<void(void)> updateCallback = static_cast<void(__stdcall *) (void)>(updatePtr.ToPointer());
		std::function<void(void)> drawCallback = static_cast<void(__stdcall *) (void)>(drawPtr.ToPointer());
		std::function<void(void)> destroyCallback = static_cast<void(__stdcall *) (void)>(destroyPtr.ToPointer());

		Engine::ENGINE_LINK_DESC linkDesc;
		linkDesc.CreateMethod = startCallback;
		linkDesc.UpdateLoop = updateCallback;
		linkDesc.DrawLoop = drawCallback;
		linkDesc.DestroyMethod = destroyCallback;

		_pCore->Initialise(width, height, windowed, linkDesc);
	}
}