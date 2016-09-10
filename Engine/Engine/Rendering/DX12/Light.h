#pragma once

namespace Engine
{
	class Light
	{
	public:
		enum LightType
		{
			Point,
			Spot,
			Directional
		};

		ENGINE_API Light(LightType lightType);
		ENGINE_API Light(std::string name, LightType lightType);
		ENGINE_API ~Light();

		LightType Type;
		Transform Transform;
		float Range;
		Colour Colour;

	private:
		static std::unordered_set<Light*> _lights;
		std::string _name;

		friend class DX12Renderer;
	};
}

