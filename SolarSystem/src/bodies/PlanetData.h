#pragma once

#include <core/Core.h>
#include <math/Transform.h>

namespace SolarSystem
{
	struct PlanetTime
	{
		uint32_t Years = 0;
		uint32_t Days = 0;
		uint32_t Hours = 24;
		uint32_t Minutes = 0;
		uint32_t Seconds = 0;

		uint32_t ToSeconds();

		template <typename A>
		std::string SaveMinimal(A &ar) const
		{
			return std::format("{}'{}'{}'{}'{}", Years, Days, Hours, Minutes, Seconds);
		}

		template <typename A>
		void LoadMinimal(A &ar, const std::string &v)
		{
			std::string token;
			std::stringstream ss(v);
			ss >> Years >> token >> Days >> token >> Hours >> token >> Minutes >> token >> Seconds;
		}
	};

	struct PlanetData
	{
		std::string mName;
		std::string mTexturePath;
		PlanetTime mRotationTime;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(mName, mTexturePath, mRotationTime);
		}
	};
} // namespace SolarSystem