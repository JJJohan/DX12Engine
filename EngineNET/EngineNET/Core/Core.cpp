#include <vcclr.h>

using namespace System;
using namespace System::Runtime::InteropServices;

namespace EngineNET
{
	bool Core::Update()
	{
		return Engine::Core::Update();
	}

	String^ Core::GetApplicationDirectory()
	{
		return gcnew String(Engine::Core::GetApplicationDirectory());
	}

	bool Core::Running()
	{
		return Engine::Core::Running();
	}

	void Core::Exit()
	{
		Engine::Core::Exit();
	}

	void Core::Destroy()
	{
		Engine::Core::Destroy();

		if (_gchStartDelegate.IsAllocated)
		{
			_gchStartDelegate.Free();
		}

		if (_gchUpdateDelegate.IsAllocated)
		{
			_gchUpdateDelegate.Free();
		}

		if (_gchDrawDelegate.IsAllocated)
		{
			_gchDrawDelegate.Free();
		}

		if (_gchDestroyDelegate.IsAllocated)
		{
			_gchDestroyDelegate.Free();
		}
	}

	void Core::Initialise(int width, int height, bool windowed, StartFunc^ start, UpdateFunc^ update, DrawFunc^ draw, DestroyFunc^ destroy, IntPtr^ handle)
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

		_gchStartDelegate = GCHandle::Alloc(start);
		_gchUpdateDelegate = GCHandle::Alloc(update);
		_gchDrawDelegate = GCHandle::Alloc(draw);
		_gchDestroyDelegate = GCHandle::Alloc(destroy);

		HWND windowHandle = nullptr;
		if (handle != nullptr)
		{
			windowHandle = (HWND)handle->ToPointer();
		}

		Engine::Core::Initialise(width, height, windowed, linkDesc, windowHandle);
	}

	void Core::Initialise(int width, int height, bool windowed, StartFunc^ start, UpdateFunc^ update, DrawFunc^ draw, DestroyFunc^ destroy)
	{
		Initialise(width, height, windowed, start, update, draw, destroy, nullptr);
	}
}