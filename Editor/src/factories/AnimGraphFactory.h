#pragma once

#include "asset/Factory.h"

namespace BHive
{
	class AnimGraphFactory : public Factory
	{
	public:
		virtual Ref<Asset> CreateNew() override;

		bool CanCreateNew() const { return true; }

		virtual const char *GetDefaultAssetName() const { return "NewAnimGraph"; }

		REFLECTABLEV(Factory)
	};
} // namespace BHive