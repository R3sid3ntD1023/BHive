#pragma once

#include "scene/Entity.h"
#include "asset/Asset.h"

namespace BHive
{
	class Prefab : public Asset
	{
	public:
		Prefab() = default;

		Ref<Entity> CreateInstance() { return mInstance; }

	private:
		Ref<Entity> mInstance;
	};

}