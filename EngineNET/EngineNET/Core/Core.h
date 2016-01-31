#pragma once

namespace Engine
{
	class Core;
	struct ENGINE_LINK_DESC;
}

namespace EngineNET
{
	ref class String;

	public delegate void StartFunc();
	public delegate void UpdateFunc();
	public delegate void DrawFunc();
	public delegate void DestroyFunc();

	public ref class Core
	{
	public:
		static bool Update();
		static String^ GetApplicationDirectory();
		static bool Running();
		static void Exit();
		static void Destroy();
		static void Initialise(int width, int height, bool windowed, StartFunc^ start, UpdateFunc^ update, DrawFunc^ draw, DestroyFunc^ destroy);
		static void Initialise(int width, int height, bool windowed, StartFunc^ start, UpdateFunc^ update, DrawFunc^ draw, DestroyFunc^ destroy, System::IntPtr^ handle);

	private:
		static System::Runtime::InteropServices::GCHandle _gchStartDelegate;
		static System::Runtime::InteropServices::GCHandle _gchUpdateDelegate;
		static System::Runtime::InteropServices::GCHandle _gchDrawDelegate;
		static System::Runtime::InteropServices::GCHandle _gchDestroyDelegate;
	};
}