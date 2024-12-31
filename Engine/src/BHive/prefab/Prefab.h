#pragma once

#include "scene/Entity.h"
#include "asset/Asset.h"

namespace BHive
{
	class Prefab : public Asset
	{
	public:
		Prefab() = default;

		Entity CreateInstance() { return mInstance; }

		ASSET_CLASS(Prefab);

	private:
		Entity mInstance;
	};

}