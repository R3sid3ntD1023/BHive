#pragma once

#include "world/Component.h"

#define SOLAR_SYSTEM_API __declspec(dllexport)

BEGIN_NAMESPACE(BHive)

struct SOLAR_SYSTEM_API PlanetTime
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
		char token;
		std::stringstream ss(v);
		ss >> Years >> token >> Days >> token >> Hours >> token >> Minutes >> token >> Seconds;
	}
};

struct SOLAR_SYSTEM_API PlanetComponent : public Component, public ITickable
{
	PlanetComponent() = default;

	virtual void Begin() override;
	virtual void Update(float dt) override;

	PlanetTime RotationTime;
	PlanetTime OrbitalTime;

	virtual void Save(cereal::BinaryOutputArchive &ar) const override;
	virtual void Load(cereal::BinaryInputArchive &ar) override;

	REFLECTABLEV(Component)

private:
	float mTheta = 0.0f;
	float mOrbitalTheta = 0.0f;
	glm::vec3 mOrbitalOrigin{};
};

END_NAMESPACE