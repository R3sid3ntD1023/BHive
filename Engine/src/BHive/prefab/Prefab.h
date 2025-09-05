#pragma once

#include "asset/Asset.h"

namespace BHive
{
	class World;

	class BHIVE_API Prefab : public Asset
	{
	public:
		Prefab();

		const Ref<World> &GetInstance() const { return mInstance; }

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Asset)

	private:
		Ref<World> mInstance;
	};

} // namespace BHive