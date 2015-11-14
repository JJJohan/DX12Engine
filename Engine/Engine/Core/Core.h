#pragma once

#include "../Rendering/Renderer.h"
#include "../Input/Input.h"
#include "../Data/Colour.h"
#include "../Data/Quaternion.h"
#include "../Data/Vector2.h"
#include "../Data/Vector3.h"
#include "../Core/Time.h"
#include "../Utils/Console.h"
#include "../Utils/Logging.h"
#include "../Utils/SystemInfo.h"

namespace Engine
{
	class Core
	{
	public:
		static bool Update();
		static bool Render();
		static void Exit();
		static void Destroy();
		static bool Running;
	};
}
