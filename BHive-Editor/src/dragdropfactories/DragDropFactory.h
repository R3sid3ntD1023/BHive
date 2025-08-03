#pragma once

#include "core/Core.h"
#include "asset/AssetMetaData.h"
#include "core/math/Transform.h"

namespace BHive
{
	class World;
	class GameObject;
	class Asset;

	class DragDropFactory
	{
	public:
		virtual Ref<GameObject> CreateEntityFrom(const Ref<Asset> &asset, const std::string &name, World *world, const FTransform &transform);

		virtual bool CanCreateEntityFrom(const rttr::type &type) { return false; }

		virtual void PostCreateEntity(const Ref<Asset> &asset, Ref<GameObject> &object) {};

		static Ref<DragDropFactory> GetDragDropFactory(const rttr::type &type);

		REFLECTABLEV()
	};

} // namespace BHive