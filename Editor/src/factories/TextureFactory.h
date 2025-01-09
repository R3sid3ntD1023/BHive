#pragma once

#include "asset/Factory.h"

namespace BHive
{
	class BHIVE TextureFactory : public Factory
	{
	public:
		virtual Ref<Asset> Import(const std::filesystem::path &path) override;

		REFLECTABLEV(Factory)
	};

	REFLECT_EXTERN(TextureFactory)
} // namespace BHive
