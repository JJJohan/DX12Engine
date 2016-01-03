#pragma once

#include <string>

namespace Engine
{
	class Processor
	{
	public:
		Processor();
		void Refresh();
		std::string GetName();
		std::string GetFeatures();
		int GetL1CacheSize();
		int GetL2CacheSize();
		int GetLogicalCores();
		int GetFrequency();
		double GetClockSpeed();

	private:
		std::string _features;
		std::string _name;
		int _l1Cache;
		int _l2Cache;
		int _cores;
		int _frequency;
	};
}

