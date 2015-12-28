#pragma once

#include <emmintrin.h>

namespace Engine
{
	inline float min(float a, float b)
	{
		// Branchless SSE min.
		_mm_store_ss(&a, _mm_min_ss(_mm_set_ss(a), _mm_set_ss(b)));
		return a;
	}

	inline float max(float a, float b)
	{
		// Branchless SSE max.
		_mm_store_ss(&a, _mm_max_ss(_mm_set_ss(a), _mm_set_ss(b)));
		return a;
	}

	inline float clamp(float val, float minval, float maxval)
	{
		// Branchless SSE clamp.
		// return minss( maxss(val,minval), maxval );

		_mm_store_ss(&val, _mm_min_ss(_mm_max_ss(_mm_set_ss(val), _mm_set_ss(minval)), _mm_set_ss(maxval)));
		return val;
	}
}
