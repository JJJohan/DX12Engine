#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport) 
#else
#define ENGINE_API __declspec(dllimport) 
#endif

/*

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

//#include <d3d12.h>
//#include <DirectXMath.h>
//#include <D3Dcompiler.h>
//#include "Rendering/DX12/d3dx12.h"

#include "Data/Colour.h"
#include "Data/Vector2.h"
#include "Data/Vector3.h"
#include "Data/Quaternion.h"
#include "Data/String.h"
#include "Data/Transform.h"

//#include "Core/Core.h"
//#include "Core/Time.h"

//#include "Factory/ResourceFactory.h"

//#include "Input/Input.h"

*/