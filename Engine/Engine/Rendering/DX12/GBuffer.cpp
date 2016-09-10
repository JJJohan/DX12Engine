#include "GBuffer.h"
#include "Texture.h"
#include "Camera.h"
#include <dxgi1_4.h>
#include "DX12Renderer.h"
#include "CommandQueue.h"
#include "Material.h"
#include "VertexBufferInstance.h"
#include "IndexBufferInstance.h"

namespace Engine
{
	const GBuffer::BufferFormat GBuffer::Buffers[GBUFFER_NUM_BUFFERS] =
	{
		{ GBUFFER_BUFFER_TYPE_ALBEDO_ROUGHNESS, DXGI_FORMAT_R8G8B8A8_UNORM },
		{ GBUFFER_BUFFER_TYPE_NORMAL_METALLIC, DXGI_FORMAT_R8G8B8A8_UNORM },
		{ GBUFFER_BUFFER_TYPE_TEXCOORD, DXGI_FORMAT_R8G8_UNORM },
		{ GBUFFER_BUFFER_TYPE_DEPTH, DXGI_FORMAT_R8_UNORM },
	};

	GBuffer::GBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12DescriptorHeap* srvHeap, IDXGISwapChain3* swapChain)
		: _pRtvHeap(nullptr)
		, _pSrvHeap(srvHeap)
		, _pDevice(device)
		, _pCommandList(commandList)
		, _pSwapChain(swapChain)
		, _pScreenMaterial(nullptr)
		, _screenWidth(0)
		, _screenHeight(0)
	{
		// Update screen resolution.
		Resize();

		// Initialise resources.
		CreateHeaps();
		CreateTextures();
		CreateScreenQuad();
	}

	GBuffer::~GBuffer()
	{
		// Release textures.
		for (int i = 0; i < GBUFFER_NUM_BUFFERS; ++i)
		{
			delete _pTextures[i];
			_pTextures[i] = nullptr;
		}

		// Release quads.
		for (int i = 0; i < GBUFFER_NUM_BUFFERS; ++i)
		{
			delete _pScreenQuads[i];
			_pScreenQuads[i] = nullptr;
		}

		// Release screen resources.
		delete _pScreenMaterial;

		// Release heaps.
		_pRtvHeap->Release();
	}

	void GBuffer::Resize()
	{
		// Get screen info.
		_screenWidth = Renderer::GetRenderer()->ScreenWidth();
		_screenHeight = Renderer::GetRenderer()->ScreenHeight();
		_screenRect.right = LONG(_screenWidth);
		_screenRect.bottom = LONG(_screenHeight);
	}

	void GBuffer::CreateHeaps()
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = GBUFFER_NUM_BUFFERS;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		LOGFAILEDCOM(_pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_pRtvHeap)));
		_rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_pRtvHeap->GetCPUDescriptorHandleForHeapStart());
	}

	void GBuffer::CreateScreenQuad()
	{
		// Initialise buffers
		std::vector<Vertex> vertices =
		{
			Vertex(Vector3(-1.0f, 1.0f, 0.0f), Colour::White, Vector2(0.0f, 0.0f)), // bottom left
			Vertex(Vector3(1.0f, -1.0f, 0.0f), Colour::White, Vector2(1.0f, 1.0f)), // top right
			Vertex(Vector3(1.0f, 1.0f, 0.0f), Colour::White, Vector2(1.0f, 0.0f)), // bottom right
			Vertex(Vector3(-1.0f, -1.0f, 0.0f), Colour::White, Vector2(0.0f, 1.0f)) // top left
		};

		std::vector<int> indices =
		{
			1, 0, 2,
			0, 1, 3
		};

		// Initialise screen quad
		for (int i = 0; i < GBUFFER_NUM_BUFFERS; ++i)
		{
			Material* material = ResourceFactory::CreateMaterial();
			material->LoadPixelShader(GetRelativePath("Shaders\\Quad.hlsl"), "PSMain", "ps_5_1");
			material->LoadVertexShader(GetRelativePath("Shaders\\Quad.hlsl"), "VSMain", "vs_5_1");
			material->Finalise(Material::Default_Input_Layout, false, false);

			VertexBufferInstance* vertexBuffer = ResourceFactory::CreateVertexBufferInstance(VERTEX_POS_COL_UV);
			vertexBuffer->SetVertices(vertices);

			IndexBufferInstance* indexBuffer = ResourceFactory::CreateIndexBufferInstance();
			indexBuffer->SetIndices(indices);

			_pScreenQuads[i] = new RenderObject();
			_pScreenQuads[i]->SetVertexBuffer(vertexBuffer);
			_pScreenQuads[i]->SetIndexBuffer(indexBuffer);
			_pScreenQuads[i]->SetMaterial(material);
			_pScreenQuads[i]->Transform.SetIs2D(true);
			_pScreenQuads[i]->Transform.SetScale(0.5f, 0.5f, 1.0f);
		}

		_pScreenQuads[0]->Transform.SetPosition(-0.5f, 0.5f, 0.0f);
		_pScreenQuads[1]->Transform.SetPosition(0.5f, 0.5f, 0.0f);
		_pScreenQuads[2]->Transform.SetPosition(-0.5f, -0.5f, 0.0f);
		_pScreenQuads[3]->Transform.SetPosition(0.5f, -0.5f, 0.0f);

		_pScreenQuads[0]->GetMaterial()->SetTexture(_pTextures[0]);
		_pScreenQuads[1]->GetMaterial()->SetTexture(_pTextures[1]);
		_pScreenQuads[2]->GetMaterial()->SetTexture(_pTextures[2]);
		_pScreenQuads[3]->GetMaterial()->SetTexture(_pTextures[3]);

		_pScreenQuads[0]->GetMaterial()->SetDepthMode(false, ALWAYS);
		_pScreenQuads[1]->GetMaterial()->SetDepthMode(false, ALWAYS);
		_pScreenQuads[2]->GetMaterial()->SetDepthMode(false, ALWAYS);
		_pScreenQuads[3]->GetMaterial()->SetDepthMode(false, ALWAYS);
	}

	void GBuffer::CreateTextures()
	{
		// Create textures.
		for (int i = 0; i < GBUFFER_NUM_BUFFERS; ++i)
		{
			// Create a resource description for the textures.
			D3D12_RESOURCE_DESC textureDesc = {};
			textureDesc.Format = Buffers[i].Format;
			textureDesc.MipLevels = 1;
			textureDesc.Width = _screenWidth;
			textureDesc.Height = _screenHeight;
			textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			textureDesc.DepthOrArraySize = 1;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

			D3D12_CLEAR_VALUE* texClearVal = new D3D12_CLEAR_VALUE();
			texClearVal->Color[0] = 0.0f;
			texClearVal->Color[1] = 0.0f;
			texClearVal->Color[2] = 0.0f;
			texClearVal->Color[3] = 1.0f;

			texClearVal->Format = textureDesc.Format;
			_pTextures[i] = ResourceFactory::CreateTexture(_screenWidth, _screenHeight);
			_pTextures[i]->SetResourceDescription(textureDesc);
			_pTextures[i]->SetHeapDescription(D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, texClearVal);
			_pTextures[i]->Apply();
		}

		CommandQueue::Enqueue(std::bind(&GBuffer::FinishResourceInit, this));
	}

	void GBuffer::FinishResourceInit()
	{
		// Render target view description.
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;

		// Create render textures.
		for (int i = 0; i < GBUFFER_NUM_BUFFERS; ++i)
		{
			rtvDesc.Format = Buffers[i].Format;
			_pDevice->CreateRenderTargetView(_pTextures[i]->GetResource(), &rtvDesc, _rtvHandle);
			_rtvHandle.Offset(1, D3DUtils::GetRTVDescriptorSize());
		}
	}

	ID3D12DescriptorHeap* GBuffer::GetRTVHeap() const
	{
		return _pRtvHeap;
	}
	
	void GBuffer::Clear(ID3D12DescriptorHeap* dsvHeap)
	{
		// Set the viewport.
		_pCommandList->RSSetViewports(1, &Camera::Main()->GetViewPort());

		// Indicate that the GBuffer textures will be used as render targets.
		for (int i = 0; i < GBUFFER_NUM_BUFFERS; ++i)
		{
			_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_pTextures[i]->GetResource(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
		}

		// Bind the render target view array and depth stencil buffer to the output render pipeline.
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			_pRtvHeap->GetCPUDescriptorHandleForHeapStart(),
			0,
			D3DUtils::GetRTVDescriptorSize());

		_pCommandList->OMSetRenderTargets(GBUFFER_NUM_BUFFERS, &rtvHandle, TRUE, &dsvHeap->GetCPUDescriptorHandleForHeapStart());

		// Clear the render target buffers.
		FLOAT clearColour[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		_rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_pRtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (int i = 0; i < GBUFFER_NUM_BUFFERS; ++i)
		{
			_pCommandList->ClearRenderTargetView(_rtvHandle, clearColour, 0, nullptr);
			_rtvHandle.Offset(1, D3DUtils::GetRTVDescriptorSize());
		}
	}

	void GBuffer::DrawTextures()
	{
		for (int i = 0; i < GBUFFER_NUM_BUFFERS; ++i)
		{
			_pScreenQuads[i]->Update();
			_pScreenQuads[i]->Draw();
		}
	}

	void GBuffer::Present() const
	{
		// Indicate that the GBuffer textures will now be used to present.
		for (int i = 0; i < GBUFFER_NUM_BUFFERS; ++i)
		{
			_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_pTextures[i]->GetResource(),
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
		}

		DX12Renderer::Get()->BindBackBuffer();
	}
}

