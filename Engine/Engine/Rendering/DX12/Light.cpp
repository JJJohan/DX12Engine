#include "Light.h"

namespace Engine
{
	std::unordered_set<Light*> Light::_lights = std::unordered_set<Light*>();

	Light::Light(LightType lightType)
		: Light("Light", lightType)
	{
	}

	Light::Light(std::string name, LightType lightType)
		: Type(lightType)
		, Range(10.0f)
		, _name(name)
	{
	}

	Light::~Light()
	{
		auto it = _lights.find(this);
		if (it == _lights.end())
		{
			Logging::LogError("Light '{0}' was not part of lights list.", _name);
		}
		else
		{
			_lights.erase(it);
		}
	}
}