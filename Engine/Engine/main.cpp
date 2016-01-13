#include <iostream>
#include <sstream>
#include "Core/Core.h"
#include "Rendering/DX12/Material.h"
#include "Rendering/DX12/Texture.h"
#include "Rendering/DX12/RenderObject.h"
#include "Rendering/DX12/VertexBuffer.h"
#include "Rendering/DX12/IndexBuffer.h"
#include "Rendering/DX12/DX12Renderer.h"
#include "Factory/ResourceFactory.h"
#include "Utils/Helpers.h"
#include "Core/Time.h"
#include "Utils/Console.h"
#include "Utils/SystemInfo.h"

using namespace Engine;

class Game
{
public:
	Game();

	void Start();
	void Update();
	void Draw();
	void Destroy();

	DX12Renderer* Renderer;

private:
	RenderObject* _pTriangle;
	Texture* _pTexture;
	Material* _pMaterial;

	RenderObject* _pTriangle2;
	Texture* _pTexture2;
	Material* _pMaterial2;
};

Game::Game()
	: Renderer(nullptr)
	  , _pTriangle(nullptr)
	  , _pTexture(nullptr)
	  , _pMaterial(nullptr)
	  , _pTriangle2(nullptr)
	  , _pTexture2(nullptr)
	  , _pMaterial2(nullptr)
{
}

void Game::Start()
{
	// Create an example triangle object.
	std::vector<void*> triangleVertices =
		{
			new VertexPosUv(XMFLOAT3(0.0f, 0.25f, 0.0f), XMFLOAT2(0.5f, 0.0f)),
			new VertexPosUv(XMFLOAT3(0.25f, -0.25f, 0.0f), XMFLOAT2(1.0f, 64.0f)),
			new VertexPosUv(XMFLOAT3(-0.25f, -0.25f, 0.0f), XMFLOAT2(0.0f, 64.0f)),

			new VertexPosUv(XMFLOAT3(-0.25f, -0.25f, 0.0f), XMFLOAT2(0.0f, 64.0f)),
			new VertexPosUv(XMFLOAT3(0.25f, -0.25f, 0.0f), XMFLOAT2(1.0f, 64.0f)),
			new VertexPosUv(XMFLOAT3(0.0f, 0.25f, 0.0f), XMFLOAT2(0.5f, 0.0f))
		};

	VertexBufferInstance* vertexBuffer = ResourceFactory::CreateVertexBufferInstance(VERTEX_POS_UV);
	vertexBuffer->SetVertices(triangleVertices);

	_pTexture = ResourceFactory::CreateTexture();
	_pTexture->Load("C:\\Users\\JJJohan\\Source\\Repos\\DX12Engine\\Engine\\Build\\Textures\\font.dds");

	_pMaterial = ResourceFactory::CreateMaterial();
	_pMaterial->SetTexture(_pTexture);
	_pMaterial->LoadVertexShader(GetRelativeFilePath("Shaders\\DiffuseTexture.hlsl"), "VSMain", "vs_5_1");
	_pMaterial->LoadPixelShader(GetRelativeFilePath("Shaders\\DiffuseTexture.hlsl"), "PSMain", "ps_5_1");
	_pMaterial->Finalise(vertexBuffer->GetInputLayout());

	_pTriangle = new RenderObject();
	_pTriangle->SetVertexBuffer(vertexBuffer);
	_pTriangle->SetMaterial(_pMaterial);

	// Create an example triangle object.
	/*std::vector<Vertex*> vertices2 =
		{
			new VertexPosUv(XMFLOAT3(-0.25f, 0.25f, 0.0f), XMFLOAT2(0.0f, 0.0f)), // bottom left
			new VertexPosUv(XMFLOAT3(0.25f, -0.25f, 0.0f), XMFLOAT2(1.0f, 1.0f)), // top right
			new VertexPosUv(XMFLOAT3(0.25f, 0.25f, 0.0f), XMFLOAT2(1.0f, 0.0f)), // bottom right
			new VertexPosUv(XMFLOAT3(-0.25f, -0.25f, 0.0f), XMFLOAT2(0.0f, 1.0f)) // top left
		};

	std::vector<int> indices =
		{
			1, 0, 2,
			0, 1, 3
		};

	VertexBufferInstance* vertexBuffer2 = ResourceFactory::CreateVertexBufferInstance(VERTEX_POS_UV);
	vertexBuffer2->SetVertices(vertices2);

	IndexBuffer* indexBuffer = ResourceFactory::CreateIndexBuffer();
	indexBuffer->SetIndices(indices);

	_pTexture2 = ResourceFactory::CreateTexture();
	_pTexture2->Load("C:\\Users\\JJJohan\\Desktop\\test2.png");

	_pMaterial2 = ResourceFactory::CreateMaterial();
	_pMaterial2->SetTexture(_pTexture2);
	_pMaterial2->LoadVertexShader(GetRelativeFilePath("Shaders\\DiffuseTexture.hlsl"), "VSMain", "vs_5_1");
	_pMaterial2->LoadPixelShader(GetRelativeFilePath("Shaders\\DiffuseTexture.hlsl"), "PSMain", "ps_5_1");
	_pMaterial2->Finalise(vertexBuffer2->GetInputLayout());

	_pTriangle2 = new RenderObject();
	_pTriangle2->SetIndexBuffer(indexBuffer);
	_pTriangle2->SetVertexBuffer(vertexBuffer2);
	_pTriangle2->SetMaterial(_pMaterial2);*/
}

void Game::Update()
{
	static float timer = 0.0f;
	static bool done = false;
	timer += Time::DeltaTime();

	if (!done && timer > 2.0f)
	{
		done = true;
	}

	if (_pTriangle != nullptr)
	{
		_pTriangle->Transform.Rotate(XMQuaternionRotationRollPitchYaw(45.0f * Time::DeltaTime(), 35.0f * Time::DeltaTime(), 25.0f * Time::DeltaTime()));
	}

	if (_pTriangle2 != nullptr)
	{
		_pTriangle2->Transform.SetPosition(cos(timer + sin(timer)), sin(timer), 0.0f);
	}
}

void Game::Draw()
{
	if (_pTriangle != nullptr)
	{
		_pTriangle->Draw();
	}

	if (_pTriangle2 != nullptr)
	{
		_pTriangle2->Draw();
	}
}

void Game::Destroy()
{
	delete _pMaterial;
	delete _pTriangle;
	delete _pTexture;

	delete _pMaterial2;
	delete _pTriangle2;
	delete _pTexture2;
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
#ifdef _DEBUG
	Engine::Console::InitConsole();
#endif

	Engine::Logging::Log("Starting engine...\n");

	Engine::SystemInfo::PrintSystemInfo();

	Engine::IRenderer* renderer = Engine::Renderer::CreateRenderer(Engine::RenderAPI::Direct3D12);

	Engine::SystemInfo::PrintSystemInfo();

	float timer = 0.0f;
	Game game;
	game.Renderer = static_cast<DX12Renderer*>(renderer);

	ENGINE_LINK_DESC engineLink =
		{
			std::bind(&Game::Start, &game),
			std::bind(&Game::Update, &game),
			std::bind(&Game::Draw, &game),
			std::bind(&Game::Destroy, &game)
		};

	Engine::Core::Initialise(1024, 768, true, engineLink);

	while (Engine::Core::Running())
	{
		if (Engine::Core::Update() != EXIT_SUCCESS)
		{
			// Update returned false, exit.
			break;
		}

		timer += Engine::Time::DeltaTime();
		if (timer > 1.0f)
		{
			std::stringstream fps;
			fps << "Engine - CPU: " << int(1 / Engine::Time::DeltaTime()) << " FPS | GPU: " << int(1 / Engine::Time::GPUTime()) << " FPS";
			renderer->SetWindowTitle(fps.str());
			timer -= 1.0f;
		}
	}

	Engine::Logging::Log("\n\nDestroying engine...");

	Engine::Core::Destroy();

	Engine::Logging::Log("Engine destroyed");

	return EXIT_SUCCESS;
}

