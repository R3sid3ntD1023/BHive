#pragma once

#include <random>

namespace BHive
{
	class ResourceID
	{
	public:
		ResourceID()
			: mValue(Generate())
		{

		}
		
		explicit ResourceID(uint64_t v)
			: mValue(v)
		{

		}

		operator uint64_t() const { return mValue; }

		bool operator==(const ResourceID &other) const { return mValue == other.mValue; }

	private:

		uint64_t mValue{0};

		static uint64_t Generate() 
		{
			static thread_local std::mt19937_64 rng(Seed());
			static thread_local std::uniform_int_distribution<uint64_t> dist;

			return dist(rng);
		}

		static uint64_t Seed()
		{
			std::random_device dev;
			uint64_t seed = 0;

			for (int i = 0; i < 4; i++)
				seed = (seed << 16) ^ dev();

			return seed;
		}
	};	
}

namespace std
{
	template<>
	struct hash<BHive::ResourceID>
	{
		size_t operator()(const BHive::ResourceID &id) const noexcept { return std::hash<uint64_t>()((uint64_t)id); }
	};
}