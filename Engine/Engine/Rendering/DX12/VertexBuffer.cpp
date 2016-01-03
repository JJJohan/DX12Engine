#include "VertexBuffer.h"

namespace Engine
{
	VertexBufferBase::VertexBufferBase()
		: _vertexCount(0)
		, _vertexBufferView()
	{
	}

	void VertexBufferBase::Bind(ID3D12GraphicsCommandList* commandList) const
	{
		commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
	}

	const std::vector<D3D12_INPUT_ELEMENT_DESC>& VertexBufferBase::GetInputLayout() const
	{
		return _inputLayout;
	}

	int VertexBufferBase::Count() const
	{
		return int(_vertexCount);
	}
}

