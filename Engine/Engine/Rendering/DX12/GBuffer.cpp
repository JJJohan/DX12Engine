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
	GBuffer::GBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12DescriptorHeap* srvHeap, IDXGISwapChain3* swapChain)
		: _pRtvHeap(nullptr)
		, _pSrvHeap(srvHeap)
		, _pDevice(device)
		, _pCommandList(commandList)
		, _pSwapChain(swapChain)
		, _pDepthTexture(nullptr)
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
		for (int i = 0; i < GBUFFER_NUM_TEXTURES; ++i)
		{
			delete _pTextures[i];
			_pTextures[i] = nullptr;
		}

		// Release quads.
		for (int i = 0; i < GBUFFER_NUM_TEXTURES + 1; ++i)
		{
			delete _pScreenQuads[i];
			_pScreenQuads[i] = nullptr;
		}

		// Release depth texture.
		delete _pDepthTexture;
		_pDepthTexture = nullptr;

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
		rtvHeapDesc.NumDescriptors = GBUFFER_NUM_TEXTURES;
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
		for (int i = 0; i < GBUFFER_NUM_TEXTURES + 1; ++i)
		{
			Material* material = ResourceFactory::CreateMaterial();
			material->LoadPixelShader(GetRelativePath("Shaders\\Quad.hlsl"), "PSMain", "ps_5_1");
			material->LoadVertexShader(GetRelativePath("Shaders\\Quad.hlsl"), "VSMain", "vs_5_1");
			material->Finalise(Material::Default_Input_Layout, false, false);

			VertexBufferInstance* vertexBuffer = ResourceFactory::CreateVertexBufferInstance();
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
		_pScreenQuads[3]->GetMaterial()->SetTexture(_pDepthTexture);
	}

	void GBuffer::CreateTextures()
	{
		// Create a resource description for the textures.
		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Width = _screenWidth;
		textureDesc.Height = _screenHeight;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		D3D12_DEPTH_STENCIL_VALUE texStencilVal;
		texStencilVal.Depth = 1;
		texStencilVal.Stencil = 0;
		D3D12_CLEAR_VALUE* texClearVal = new D3D12_CLEAR_VALUE();
		texClearVal->Format = textureDesc.Format;
		texClearVal->DepthStencil = texStencilVal;
		texClearVal->Color[0] = 0.0f;
		texClearVal->Color[1] = 0.0f;
		texClearVal->Color[2] = 0.0f;
		texClearVal->Color[3] = 1.0f;

		// Create textures.
		for (int i = 0; i < GBUFFER_NUM_TEXTURES; ++i)
		{
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
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;

		// Create render textures.
		for (int i = 0; i < GBUFFER_NUM_TEXTURES; ++i)
		{
			_pDevice->CreateRenderTargetView(_pTextures[i]->GetResource(), &rtvDesc, _rtvHandle);
			_rtvHandle.Offset(1, D3DUtils::GetRTVDescriptorSize());
		}
	}

	ID3D12DescriptorHeap* GBuffer::GetRTVHeap() const
	{
		return _pRtvHeap;
	}

	void GBuffer::Write() const
	{
		// Indicate that the GBuffer textures will be used as render targets.
		for (int i = 0; i < GBUFFER_NUM_TEXTURES; ++i)
		{
			_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_pTextures[i]->GetResource(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
		}
		
		// Bind the render target view array and depth stencil buffer to the output render pipeline.
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			_pRtvHeap->GetCPUDescriptorHandleForHeapStart(),
			0,
			D3DUtils::GetRTVDescriptorSize());
		_pCommandList->OMSetRenderTargets(GBUFFER_NUM_TEXTURES, &rtvHandle, TRUE, &DX12Renderer::Get()->GetDepthBufferHeap()->GetCPUDescriptorHandleForHeapStart());

		// Set the viewport.
		_pCommandList->RSSetViewports(1, &Camera::Main()->GetViewPort());
	}

	void GBuffer::Clear()
	{
		// Clear the render target buffers.
		FLOAT clearColour[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		_rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_pRtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (int i = 0; i < GBUFFER_NUM_TEXTURES; ++i)
		{
			_pCommandList->ClearRenderTargetView(_rtvHandle, clearColour, 1, &_screenRect);
			_rtvHandle.Offset(1, D3DUtils::GetRTVDescriptorSize());
		}
	}

	void GBuffer::DrawTextures()
	{
		for (int i = 0; i < GBUFFER_NUM_TEXTURES + 1; ++i)
		{
			_pScreenQuads[i]->Update();
			_pScreenQuads[i]->Draw();
		}
	}

	void GBuffer::Present() const
	{
		// Indicate that the GBuffer textures will now be used to present.
		for (int i = 0; i < GBUFFER_NUM_TEXTURES; ++i)
		{
			_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_pTextures[i]->GetResource(),
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
		}

		DX12Renderer::Get()->BindBackBuffer();
	}
}

