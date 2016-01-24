#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport) 
#else
#define ENGINE_API __declspec(dllimport) 
#endif

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <unordered_set>
#include <mutex>
#include <memory>
#include <thread>

#include <d3d12.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include "Rendering/DX12/d3dx12.h"

#include "Utils/Logging.h"
#include "Utils/Helpers.h"
#include "Utils/Console.h"
#include "Utils/SystemInfo.h"
#include "Utils/Math.h"

#include "Data/Colour.h"
#include "Data/Vector2.h"
#include "Data/Vector3.h"
#include "Data\Vector4.h"
#include "Data/Quaternion.h"
#include "Data/String.h"
#include "Data/Transform.h"

#include "Core/Core.h"
#include "Core/Time.h"

#include "Factory/ResourceFactory.h"

/*#include "Rendering/DX12/BufferBucket.h"
#include "Rendering/DX12/BufferInstance.h"
#include "Rendering/DX12/ConstantBuffer.h"
#include "Rendering/DX12/VertexBuffer.h"
#include "Rendering/DX12/IndexBuffer.h"

#include "Rendering/DX12/Text.h"
#include "Rendering/DX12/FontManager.h"
#include "Rendering/DX12/Font.h"
#include "Rendering/DX12/Material.h"
#include "Rendering/DX12/Texture.h"
#include "Rendering/DX12/RenderObject.h"
#include "Rendering/DX12/DX12Renderer.h"
#include "Rendering/DX12/Camera.h"
#include "Rendering/DX12/HeapManager.h"
#include "Rendering/DX12/HeapResource.h"
#include "Rendering/DX12/CommandQueue.h"*/

#include "Input/Input.h"