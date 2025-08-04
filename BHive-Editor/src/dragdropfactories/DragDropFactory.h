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
		virtual Ref<GameObject> CreateFrom(const Ref<Asset> &asset, const std::string &name, World *world, const FTransform &transform);

		virtual bool CanCreate(const rttr::type &type) { return false; }

	protected:
		virtual void PostCreate(const Ref<Asset> &asset, Ref<GameObject> &object) {};

	public:
		static Ref<DragDropFactory> GetFactoryFromType(const rttr::type &type);

		template <typename TFactory>
		static void RegisterDragDropFactory(const rttr::type &type)
		{
			RegisterFactory(type, CreateRef<TFactory>());
		}

	private:
		static void RegisterFactory(const rttr::type &type, const Ref<DragDropFactory> &factory);

		static DragDropFactoryRegistry &GetRegistry();
	};

} // namespace BHive