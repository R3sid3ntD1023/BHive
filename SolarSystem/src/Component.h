#pragma once

#include <core/Core.h>
#include <core/reflection/Reflection.h>
#include <core/serialization/Serialization.h>

struct CelestrialBody;

struct Component
{
	virtual void Begin() {}

	virtual void Update(float dt) {};

	virtual void Save(cereal::JSONOutputArchive &ar) const {};

	virtual void Load(cereal::JSONInputArchive &ar) {};

	void SetOwner(CelestrialBody *owner);

	void SetTickEnabled(bool enabled);

	bool IsTickEnabled() const { return mTickEnabled; }

	CelestrialBody *GetOwner() const { return mOwner; }

	REFLECTABLEV()

private:
	CelestrialBody *mOwner = nullptr;
	bool mTickEnabled = true;
};