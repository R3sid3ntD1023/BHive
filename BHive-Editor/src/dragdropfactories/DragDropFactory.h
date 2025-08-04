#pragma once

#include "core/Core.h"
#include "asset/AssetMetaData.h"
#include "core/math/Transform.h"

namespace BHive
{
	class World;
	class GameObject;
	class Asset;
	class DragDropFactoryRegistry;

	class DragDropFactory
	{
	public:
		virtual Ref<GameObject> create_from(const Ref<Asset> &asset, const std::string &name, World *world, const FTransform &transform);

		virtual bool can_create(const rttr::type &type) { return false; }

	protected:
		virtual void post_create(const Ref<Asset> &asset, Ref<GameObject> &object) {};

	public:
		static Ref<DragDropFactory> get_factory_from_type(const rttr::type &type);

		template <typename TFactory>
		static void register_drag_drop_factory(const rttr::type &type)
		{
			register_factory(type, CreateRef<TFactory>());
		}

	private:
		static void register_factory(const rttr::type &type, const Ref<DragDropFactory> &factory);

		static DragDropFactoryRegistry &get_registry();
	};

} // namespace BHive