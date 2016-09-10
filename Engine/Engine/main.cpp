#include "Rendering/DX12/Material.h"
#include "Rendering/DX12/Texture.h"
#include "Rendering/DX12/RenderObject.h"
#include "Rendering/DX12/DX12Renderer.h"
#include "Rendering/DX12/Text.h"
#include "Rendering/DX12/FontManager.h"
#include "Rendering/DX12/Font.h"
#include "Rendering/DX12/VertexBufferInstance.h"
#include "Rendering/DX12/IndexBufferInstance.h"
#include "Rendering/DX12/Primitives.h"

using namespace Engine;

class Game
{
public:
	Game();

	void Start();
	void Update() const;
	void Draw() const;
	void Destroy() const;

	DX12Renderer* Renderer;

private:
	void RemoveTriangles();

	RenderObject* _pTriangle;
	Texture* _pTexture;
	Material* _pMaterial;

	RenderObject* _pTriangle2;
	Texture* _pTexture2;
	Material* _pMaterial2;

	Engine::Font* _pFont;
	Text* _pText;
	Text* _pText2;
};

Game::Game()
	: Renderer(nullptr)
	, _pTriangle(nullptr)
	, _pTexture(nullptr)
	, _pMaterial(nullptr)
	, _pTriangle2(nullptr)
	, _pTexture2(nullptr)
	, _pMaterial2(nullptr)
	, _pFont(nullptr)
	, _pText(nullptr)
	, _pText2(nullptr) {}

void Game::RemoveTriangles()
{
	VertexBufferInstance* vBuffer = _pTriangle->GetVertexBuffer();
	std::vector<Vertex> verts = vBuffer->GetVertices();
	verts[0].Pos.x += 5.0f;
	vBuffer->SetVertices(verts);
}

void Game::Start()
{
	_pTexture = ResourceFactory::CreateTexture(0, 0);
	_pTexture->Load(GetRelativePath("Textures\\test2.png"));

	_pMaterial = ResourceFactory::CreateMaterial();
	_pMaterial->SetTexture(_pTexture);
	_pMaterial->LoadVertexShader(GetRelativePath("Shaders\\DiffuseTexture_Deferred.hlsl"), "VSMain", "vs_5_1");
	_pMaterial->LoadPixelShader(GetRelativePath("Shaders\\DiffuseTexture_Deferred.hlsl"), "PSMain", "ps_5_1");
	_pMaterial->Finalise(Material::PBR_Input_Layout);

	_pTriangle = Primitives::CreateCylinder("triangle", 8);
	_pTriangle->SetMaterial(_pMaterial);

	Engine::Input::RegisterKey('F', KeyDown, std::bind(&Game::RemoveTriangles, this), "deleteTriangles");

	// Create an example triangle object.
	std::vector<Vertex> vertices2 =
		{
			Vertex(Vector3(-0.25f, 0.25f, 0.0f), Colour::White, Vector2(0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f)), // bottom left
			Vertex(Vector3(0.25f, -0.25f, 0.0f), Colour::White, Vector2(1.0f, 1.0f), Vector3(0.0f, 0.0f, -1.0f)), // top right
			Vertex(Vector3(0.25f, 0.25f, 0.0f), Colour::White, Vector2(1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f)), // bottom right
			Vertex(Vector3(-0.25f, -0.25f, 0.0f), Colour::White, Vector2(0.0f, 1.0f), Vector3(0.0f, 0.0f, -1.0f)) // top left
		};

	std::vector<int> indices =
		{
			1, 0, 2,
			0, 1, 3
		};

	VertexBufferInstance* vertexBuffer2 = ResourceFactory::CreateVertexBufferInstance();
	vertexBuffer2->SetVertices(vertices2);

	IndexBufferInstance* indexBuffer = ResourceFactory::CreateIndexBufferInstance();
	indexBuffer->SetIndices(indices);

	_pTexture2 = ResourceFactory::CreateTexture(0, 0);
	_pTexture2->Load(GetRelativePath("Textures\\test2.png"));

	_pMaterial2 = ResourceFactory::CreateMaterial();
	_pMaterial2->SetTexture(_pTexture2);
	_pMaterial2->LoadVertexShader(GetRelativePath("Shaders\\DiffuseTexture_Deferred.hlsl"), "VSMain", "vs_5_1");
	_pMaterial2->LoadPixelShader(GetRelativePath("Shaders\\DiffuseTexture_Deferred.hlsl"), "PSMain", "ps_5_1");
	_pMaterial2->Finalise(vertexBuffer2->GetInputLayout());

	_pTriangle2 = new RenderObject("triangle2");
	_pTriangle2->SetIndexBuffer(indexBuffer);
	_pTriangle2->SetVertexBuffer(vertexBuffer2);
	_pTriangle2->SetMaterial(_pMaterial2);

	_pFont = FontManager::LoadFont("Myriad", GetRelativePath("Textures\\myriad.dds"), GetRelativePath("Textures\\myriad.txt"));
	_pText = new Text("text", _pFont);
	_pText->SetColour(Colour::Yellow);
	_pText->Transform.SetPosition(0.0f, 0.0f, 0.0f);
	_pText2 = new Text("text2", _pFont);
	_pText2->SetText("Hello world!");
	_pText2->Transform.SetPosition(0.0f, 20.0f, 0.0f);
	_pText2->EnableWorldSpace(true);
}

void Game::Update() const
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
		_pTriangle2->Transform.SetPosition(cos(timer), sin(timer), 0.0f);
	}

	if (_pText != nullptr)
	{
		Vector3 euler = Camera::Main()->Transform.GetRotation().GetEulerAngles();
		String text = String::Format("X: {0} . Y: {1} . Z: {2}", euler.X, euler.Y, euler.Z);
		_pText->SetText(text);
	}

	if (_pText2 != nullptr)
	{
		_pText2->Transform.SetPosition(cos(-timer + sin(timer)) / 2, sin(-timer) / 2, 0.0f);
		_pText2->Transform.Rotate(XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, 100.0f * Time::DeltaTime()));
		_pText2->Transform.SetScale(1.0f + cos(-timer) / 2, 1.0f + cos(-timer) / 2, 1.0f);
	}
}

void Game::Draw() const
{
	if (_pTriangle != nullptr)
	{
		_pTriangle->Draw();
	}

	if (_pTriangle2 != nullptr)
	{
		_pTriangle2->Draw();
	}

	if (_pText != nullptr)
	{
		_pText->Draw();
	}

	if (_pText2 != nullptr)
	{
		_pText2->Draw();
	}
}

void Game::Destroy() const
{
	delete _pMaterial;
	delete _pTriangle;
	delete _pTexture;

	delete _pMaterial2;
	delete _pTriangle2;
	delete _pTexture2;

	delete _pFont;
	delete _pText;
	delete _pText2;
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

	Engine::IRenderer* renderer = Engine::Renderer::CreateRenderer(Engine::RenderAPI::Direct3D12);

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
			String title = String::Format("Engine - CPU: {0} FPS | GPU: {1} FPS", int(1 / Time::DeltaTime()), int(1 / Time::GPUTime()));
			renderer->SetWindowTitle(title);
			timer -= 1.0f;
		}
	}

	Engine::Logging::Log("\n\nDestroying engine...");

	Engine::Core::Destroy();

	Engine::Logging::Log("Engine destroyed");

	return EXIT_SUCCESS;
}

