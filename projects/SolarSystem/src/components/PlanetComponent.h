#pragma once

#include "Component.h"

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
		char token;
		std::stringstream ss(v);
		ss >> Years >> token >> Days >> token >> Hours >> token >> Minutes >> token >> Seconds;
	}
};

struct PlanetComponent : public Component
{
	PlanetComponent() = default;

	virtual void Update(float dt) override;

	PlanetTime mTime;

	float mTheta = 0.0f;

	virtual void Save(cereal::JSONOutputArchive &ar) const override;
	virtual void Load(cereal::JSONInputArchive &ar) override;

	REFLECTABLEV(Component)
};

struct RevolutionComponent : public Component
{
	RevolutionComponent() = default;

	virtual void Begin() override;

	virtual void Update(float dt) override;

	PlanetTime mRevolutionTime;
	float mRevolutionTheta = 0.0f;

	virtual void Save(cereal::JSONOutputArchive &ar) const override;
	virtual void Load(cereal::JSONInputArchive &ar) override;

	REFLECTABLEV(Component)

private:
	glm::vec3 mOrigin{};
};