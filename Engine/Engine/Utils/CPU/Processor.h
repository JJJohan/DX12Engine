#pragma once

namespace Engine
{
	class Processor
	{
	public:
		Processor();
		void Refresh();
		std::string GetName() const;
		std::string GetFeatures() const;
		int GetL1CacheSize() const;
		int GetL2CacheSize() const;
		int GetLogicalCores() const;
		int GetFrequency() const;
		static double GetClockSpeed();

	private:
		std::string _features;
		std::string _name;
		int _l1Cache;
		int _l2Cache;
		int _cores;
		int _frequency;
	};
}

