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
		, _pDsvHeap(nullptr)
		, _pSrvHeap(srvHeap)
		, _pDevice(device)
		, _pCommandList(commandList)
		, _pSwapChain(swapChain)
		, _pDepthTexture(nullptr)
		, _pScreenQuad(nullptr)
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

		// Release depth texture.
		delete _pDepthTexture;
		_pDepthTexture = nullptr;

		// Release screen resources.
		delete _pScreenQuad;
		delete _pScreenMaterial;

		// Release heaps.
		_pRtvHeap->Release();
		_pDsvHeap->Release();
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

		// Describe and create a depth stencil view (DSV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		LOGFAILEDCOM(_pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&_pDsvHeap)));
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

		VertexBufferInstance* vertexBuffer = ResourceFactory::CreateVertexBufferInstance();
		vertexBuffer->SetVertices(vertices);

		IndexBufferInstance* indexBuffer = ResourceFactory::CreateIndexBufferInstance();
		indexBuffer->SetIndices(indices);

		// Initialise material
		_pScreenMaterial = ResourceFactory::CreateMaterial();
		_pScreenMaterial->LoadPixelShader(GetRelativePath("Shaders\\Quad.hlsl"), "PSMain", "ps_5_1");
		_pScreenMaterial->LoadVertexShader(GetRelativePath("Shaders\\Quad.hlsl"), "VSMain", "vs_5_1");
		_pScreenMaterial->Finalise(Material::Default_Input_Layout);

		// Initialise screen quad
		_pScreenQuad = new RenderObject();
		_pScreenQuad->SetVertexBuffer(vertexBuffer);
		_pScreenQuad->SetIndexBuffer(indexBuffer);
		_pScreenQuad->SetMaterial(_pScreenMaterial);
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
			_pTextures[i]->SetHeapDescription(D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_RENDER_TARGET, texClearVal);
			_pTextures[i]->Apply();
		}

		// Create a resource description for the depth texture.
		D3D12_RESOURCE_DESC depthDesc = {};
		depthDesc.MipLevels = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.Width = _screenWidth;
		depthDesc.Height = _screenHeight;
		depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		depthDesc.DepthOrArraySize = 1;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		D3D12_DEPTH_STENCIL_VALUE depthStencilVal;
		depthStencilVal.Depth = 1;
		depthStencilVal.Stencil = 0;
		D3D12_CLEAR_VALUE* depthClearVal = new D3D12_CLEAR_VALUE();
		depthClearVal->Format = depthDesc.Format;
		depthClearVal->DepthStencil = depthStencilVal;
		depthClearVal->Color[0] = 0.0f;
		depthClearVal->Color[1] = 0.0f;
		depthClearVal->Color[2] = 0.0f;
		depthClearVal->Color[3] = 1.0f;

		// Create depth texture.
		_pDepthTexture = ResourceFactory::CreateTexture(_screenWidth, _screenHeight);
		_pDepthTexture->SetResourceDescription(depthDesc);
		_pDepthTexture->SetHeapDescription(D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_DEPTH_WRITE, depthClearVal);
		_pDepthTexture->Apply();

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

		// Create a depth stencil view description.
		D3D12_DEPTH_STENCIL_VIEW_DESC stencilDesc = {};
		stencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		stencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		stencilDesc.Texture2D.MipSlice = 0;

		_pDevice->CreateDepthStencilView(_pDepthTexture->GetResource(), &stencilDesc, _pDsvHeap->GetCPUDescriptorHandleForHeapStart());
	}

	ID3D12DescriptorHeap* GBuffer::GetRTVHeap() const
	{
		return _pRtvHeap;
	}

	ID3D12DescriptorHeap* GBuffer::GetDSVHeap() const
	{
		return _pDsvHeap;
	}

	void GBuffer::Write() const
	{
		// Indicate that the GBuffer textures will be used as render targets.
		for (int i = 0; i < GBUFFER_NUM_TEXTURES; ++i)
		{
			_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_pTextures[i]->GetResource(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
		}
		_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_pDepthTexture->GetResource(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE));
		
		// Bind the render target view array and depth stencil buffer to the output render pipeline.
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			_pRtvHeap->GetCPUDescriptorHandleForHeapStart(),
			0,
			D3DUtils::GetRTVDescriptorSize());
		_pCommandList->OMSetRenderTargets(GBUFFER_NUM_TEXTURES, &rtvHandle, TRUE, &_pDsvHeap->GetCPUDescriptorHandleForHeapStart());

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

		// Clear the depth buffer.
		_pCommandList->ClearDepthStencilView(_pDsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 0.0f, 0, 1, &_screenRect);
	}

	void GBuffer::DrawTextures()
	{
		// Draw the textures to a screen space quad.
		_pScreenQuad->Transform.SetIs2D(true);
		_pScreenQuad->Transform.SetScale(0.5f, 0.5f, 1.0f);
		_pScreenQuad->Transform.SetPosition(-0.5f, 0.5f, 0.0f);
		_pScreenQuad->GetMaterial()->SetTexture(_pTextures[2]);
		_pScreenQuad->Draw();

		//_pScreenQuad->Transform.SetPosition(-0.5f, -0.5f, 0.0f);
		//_pScreenQuad->GetMaterial()->SetTexture(_pTextures[1]);
		//_pScreenQuad->Draw();

		/*_pScreenQuad->Transform.SetPosition(0.5f, 0.5f, 0.0f);
		_pScreenQuad->GetMaterial()->SetTexture(_pTextures[2]);
		_pScreenQuad->Draw();

		_pScreenQuad->Transform.SetPosition(0.5f, -0.5f, 0.0f);
		_pScreenQuad->GetMaterial()->SetTexture(_pDepthTexture);
		_pScreenQuad->Draw();*/
	}

	void GBuffer::Present() const
	{
		// Indicate that the GBuffer textures will now be used to present.
		for (int i = 0; i < GBUFFER_NUM_TEXTURES; ++i)
		{
			_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_pTextures[i]->GetResource(),
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
		}
		_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_pDepthTexture->GetResource(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		DX12Renderer::Get()->BindBackBuffer();
	}
}

