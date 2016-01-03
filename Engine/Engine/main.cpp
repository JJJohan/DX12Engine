#include <iostream>
#include <sstream>
#include "Core/Core.h"
#include "Rendering/DX12/Material.h"
#include "Rendering/DX12/Texture.h"
#include "Rendering/DX12/RenderObject.h"
#include "Rendering/DX12/VertexBuffer.h"
#include "Rendering/DX12/DX12Renderer.h"
#include "Factory/RenderObjectFactory.h"
#include "Factory/MaterialFactory.h"
#include "Factory/TextureFactory.h"
#include "Factory/VertexBufferFactory.h"
#include "Utils/Helpers.h"
#include "Rendering/DX12/CommandQueue.h"

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
};

Game::Game()
	: Renderer(nullptr)
	, _pTriangle(nullptr)
	, _pTexture(nullptr)
	, _pMaterial(nullptr)
{
	
}

void Game::Start()
{
	// Create an example triangle object.
	std::vector<VertexPosUv> triangleVertices =
	{
		{ { 0.0f, 0.25f, 0.0f },{ 0.5f, 0.0f } },
		{ { 0.25f, -0.25f, 0.0f },{ 1.0f, 64.0f } },
		{ { -0.25f, -0.25f, 0.0f },{ 0.0f, 64.0f } }
	};

	VertexBuffer<VertexPosUv>* vertexBuffer = VertexBufferFactory::CreateVertexBuffer<VertexPosUv>();
	vertexBuffer->SetVertices(triangleVertices);

	_pMaterial = MaterialFactory::CreateMaterial();
	_pMaterial->LoadVertexShader(GetRelativeFilePath("Shaders\\DiffuseTexture.hlsl"), "VSMain", "vs_5_1");
	_pMaterial->LoadPixelShader(GetRelativeFilePath("Shaders\\DiffuseTexture.hlsl"), "PSMain", "ps_5_1");
	_pMaterial->Finalise(vertexBuffer->GetInputLayout());

	_pTriangle = RenderObjectFactory::CreateRenderObject();
	_pTriangle->SetVertexBuffer(vertexBuffer);
	_pTriangle->SetMaterial(_pMaterial);

	_pTexture = TextureFactory::CreateTexture(1024, 16);
	_pTexture->LoadFromDDS("C:\\Users\\JJJohan\\Source\\Repos\\DX12Engine\\Engine\\Build\\Textures\\font.dds");
}

void Game::Update()
{
	static float timer = 0.0f;
	static bool done = false;
	timer += Time::DeltaTime();

	if (!done && timer > 5.0f)
	{
		done = true;
	}
}

void Game::Draw()
{
	if (_pTriangle != nullptr)
	{
		_pTriangle->Draw();
	}
}

void Game::Destroy()
{
	delete _pMaterial;
	delete _pTriangle;
	delete _pTexture;
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