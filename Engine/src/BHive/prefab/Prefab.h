// #pragma once
//
// #include "asset/Asset.h"
// #include "scene/Entity.h"
// #include "core/SubClassOf.h"
//
// namespace BHive
//{
//	class World;
//
//	class Prefab : public Asset
//	{
//	public:
//		Prefab();
//
//		void CreateInstance(World *world);
//
//		const Ref<World> &GetInstance() const { return mInstance; }
//
//		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
//
//		virtual void Load(cereal::BinaryInputArchive &ar) override;
//
//		REFLECTABLEV(Asset)
//
//	private:
//		Ref<World> mInstance;
//	};
//
// } // namespace BHive