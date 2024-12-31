#pragma once

#include "asset/Factory.h"

namespace BHive
{
	class BHIVE TextureFactory : public Factory
	{
	public:
		virtual bool Import(Ref<Asset> &asset, const std::filesystem::path &path);

		REFLECTABLEV(Factory)
	};

	REFLECT_EXTERN(TextureFactory)
} // namespace BHive
