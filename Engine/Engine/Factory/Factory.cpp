#include "Factory.h"

namespace Engine
{
	DX12Renderer* Factory::_pRenderer = nullptr;
	thread_local ID3D12CommandList* Factory::_pCommandList = nullptr;

	void Factory::AssignCommandList(ID3D12CommandList* commandList)
	{
		_pCommandList = commandList;
	}

	ID3D12CommandList* Factory::GetCommandList()
	{
		return _pCommandList;
	}
}