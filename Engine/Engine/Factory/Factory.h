#pragma once
#include "../Rendering/DX12/DX12Renderer.h"

namespace Engine
{
	class Factory
	{
	public:
		static void AssignCommandList(ID3D12CommandList* commandList);
		static ID3D12CommandList* GetCommandList();

	protected:
		static DX12Renderer* _pRenderer;
		static thread_local ID3D12CommandList* _pCommandList;

	private:
		Factory() {};

		friend class Core;
	};
}

