#pragma once

#include <core/Core.h>
#include <math/Transform.h>
#include <core/EnumAsByte.h>
#include <asset/AssetHandle.h>
#include <core/serialization/Serialization.h>

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

enum EPlanetFlags : uint32_t
{
	None = 0,
	Single_Channel_Texture = 1 << 0
};

struct PlanetData
{
	std::string mName;
	PlanetTime mRotationTime;
	BHive::TEnumAsByte<EPlanetFlags> mFlags;
	BHive::AssetHandle mTextureHandle;
	BHive::AssetHandle mMeshHandle;

	template <typename A>
	void Serialize(A &ar)
	{
		ar(MAKE_NVP("Name", mName), MAKE_NVP("DayLength", mRotationTime), MAKE_NVP("Flags", mFlags), MAKE_NVP("Texture", mTextureHandle),
		   MAKE_NVP("Mesh", mMeshHandle));
	}
};