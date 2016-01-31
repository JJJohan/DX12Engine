#include "Text.h"
#include "Material.h"
#include "FontManager.h"
#include "Font.h"
#include "ConstantBuffer.h"
#include "VertexBufferInstance.h"
#include "IndexBufferInstance.h"

using namespace DirectX;
using namespace std::placeholders;

namespace Engine
{
	std::mutex _textChangeMutex;

	Text::Text(std::string fontName)
		: _worldSpace(false)
		  , _textDirty(false)
		  , _colDirty(false)
	{
		_pFont = FontManager::GetFont(fontName);
		RegisterTransform();
	}

	Text::Text(Font* font)
		: _worldSpace(false)
		  , _pFont(font)
		  , _textDirty(false)
		  , _colDirty(false)
	{
		RegisterTransform();
	}

	Text::Text(std::string name, std::string fontName)
		: RenderObject(name)
		, _worldSpace(false)
		, _textDirty(false)
		, _colDirty(false)
	{
		_pFont = FontManager::GetFont(fontName);
		RegisterTransform();
	}

	Text::Text(std::string name, Font* font)
		: RenderObject(name)
		, _worldSpace(false)
		, _pFont(font)
		, _textDirty(false)
		, _colDirty(false)
	{
		RegisterTransform();
	}

	void Text::RegisterTransform()
	{
		Transform.SetPosition(-1.0f, 1.0f, 0.0f);

		Transform.SetPosOverride(std::bind(&Text::SetPosition, this, _1, _2, _3));
		Transform.SetScaleOverride(std::bind(&Text::SetScale, this, _1, _2, _3));
		Transform.GetPosOverride(std::bind(&Text::GetPosition, this, _1, _2, _3));
		Transform.GetScaleOverride(std::bind(&Text::GetScale, this, _1, _2, _3));
	}

	Text::~Text()
	{
		delete _pMaterial;
	}

	void Text::Draw()
	{
		if (_text.empty())
		{
			return;
		}

		RenderObject::Draw();
	}

	void Text::Update()
	{
		if (_textDirty || _colDirty)
		{
			ChangeText();
			_textDirty = false;
			_colDirty = false;
		}

		if (_text.empty())
		{
			return;
		}

		if (_worldSpace)
		{
			RenderObject::Update();
		}
		else
		{
			XMMATRIX world = XMMatrixTranspose(Transform.GetMatrix());
			XMFLOAT4X4 worldT;
			XMStoreFloat4x4(&worldT, world);
			_pCbuffer->SetMatrix("world", worldT);
			_pVertexBuffer->SetBufferIndex(_pCbuffer->GetIndex());
		}
	}

	void Text::SetText(std::string text)
	{
		if (!_textDirty && text != _text)
		{
			_textChangeMutex.lock();
			_text = text;
			_textDirty = true;
			_textChangeMutex.unlock();
		}
	}

	void Text::ChangeText()
	{
		if (_text.empty())
		{
			return;
		}

		if (_pVertexBuffer == nullptr)
		{
			_pVertexBuffer = ResourceFactory::CreateVertexBufferInstance(VERTEX_POS_UV);
			_pIndexBuffer = ResourceFactory::CreateIndexBufferInstance();
			_pMaterial = ResourceFactory::CreateMaterial();
			_pMaterial->SetTexture(_pFont->_pTexture);
			_pMaterial->LoadVertexShader(GetRelativePath("Shaders\\Font.hlsl"), "VSMain", "vs_5_1");
			_pMaterial->LoadPixelShader(GetRelativePath("Shaders\\Font.hlsl"), "PSMain", "ps_5_1");
			_pMaterial->Finalise(_pVertexBuffer->GetInputLayout(), true);
		}

		size_t letters = _text.size();

		Vector3 scale = Transform.GetScale();

		// Update vertex buffer
		float offset = 0.0f;
		std::vector<Vertex> vertices;
		float topUv = _pFont->_topUv;
		float bottomUv = _pFont->_bottomUv;
		float widthScale, heightScale;
		if (_worldSpace)
		{
			widthScale = _pFont->_widthScale * scale.X / 100;
			heightScale = _pFont->_heightScale * scale.Y / 100;
		}
		else
		{
			widthScale = _pFont->_widthScale * scale.X * 10 / Renderer::GetRenderer()->ScreenWidth();
			heightScale = _pFont->_heightScale * scale.Y * 10 / Renderer::GetRenderer()->ScreenHeight();
		}
		for (int i = 0; i < letters; ++i)
		{
			Font::Letter l = _pFont->GetLetter(_text[i]);
			float uv1 = l.LeftUv;
			float uv2 = l.RightUv;
			float width = l.Width * widthScale;
			vertices.push_back(Vertex(Vector3(0.0f + offset, -heightScale, 0.0f), _colour, Vector2(uv1, bottomUv))); // top left
			vertices.push_back(Vertex(Vector3(0.0f + offset, 0.0f, 0.0f), _colour, Vector2(uv1, topUv))); // bottom left
			vertices.push_back(Vertex(Vector3(width + offset, -heightScale, 0.0f), _colour, Vector2(uv2, bottomUv))); // top right
			vertices.push_back(Vertex(Vector3(width + offset, 0.0f, 0.0f), _colour, Vector2(uv2, topUv))); // bottom right

			offset += width;
		}

		// Updated index buffer
		std::vector<int> indices;
		size_t idxCount = letters * 4;
		for (int i = 0; i < idxCount; i += 4)
		{
			indices.push_back(i + 0);
			indices.push_back(i + 1);
			indices.push_back(i + 3);

			indices.push_back(i + 0);
			indices.push_back(i + 3);
			indices.push_back(i + 2);
		}

		_pIndexBuffer->SetIndices(indices);
		_pVertexBuffer->SetVertices(vertices);
	}

	void Text::SetColour(Colour colour)
	{
		_colour = colour;
		_colDirty = true;
	}

	Vector3 Text::SetScale(float x, float y, float z) const
	{
		if (_worldSpace)
		{
			return Vector3(x, y, z);
		}

		return Vector3(sqrt(x), sqrt(y), 1.0f);
	}

	Vector3 Text::GetScale(float x, float y, float z) const
	{
		if (_worldSpace)
		{
			return Vector3(x, y, z);
		}

		return Vector3(x, y, z);
	}

	Vector3 Text::SetPosition(float x, float y, float z) const
	{
		if (_worldSpace)
		{
			return Vector3(x, y, z);
		}

		return Vector3(x / Renderer::GetRenderer()->ScreenWidth() * 2 - 1.0f, -y / Renderer::GetRenderer()->ScreenHeight() * 2 + 1.0f, 0.0f);
	}

	Vector3 Text::GetPosition(float x, float y, float z) const
	{
		if (_worldSpace)
		{
			return Vector3(x, y, z);
		}

		return Vector3(-x * Renderer::GetRenderer()->ScreenWidth() * 2 + 1.0f, -y * Renderer::GetRenderer()->ScreenHeight() * 2 - 1.0f, 0.0f);
	}

	void Text::EnableWorldSpace(bool flag)
	{
		_worldSpace = flag;
	}

	const Colour& Text::GetColour() const
	{
		return _colour;
	}

	std::string Text::GetText() const
	{
		return _text;
	}
}

