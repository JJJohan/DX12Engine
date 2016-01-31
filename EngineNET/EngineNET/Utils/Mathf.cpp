#include "Mathf.h"
#include <math.h>

namespace EngineNET
{
	float Mathf::Cos(float value)
	{
		return cosf(value);
	}

	float Mathf::Sin(float value)
	{
		return sinf(value);
	}

	float Mathf::Tan(float value)
	{
		return tanf(value);
	}
}