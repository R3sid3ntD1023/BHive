#include "DragDropFactory.h"
#include "asset/AssetManager.h"
#include "asset/EditorAssetManager.h"
#include "project/Project.h"
#include "world/GameObject.h"
#include "world/World.h"
#include "DragDropFactories.h"

// assets
#include "audio/AudioSource.h"
#include "mesh/SkeletalMesh.h"
#include "mesh/StaticMesh.h"
#include "prefab/Prefab.h"

namespace BHive
{
	struct DragDropFactoryRegistry
	{
		DragDropFactoryRegistry()
		{
			register_factory<StaticMesh, DragDropStaticMesh>();
			register_factory<SkeletalMesh, DragDropSkeletalMesh>();
			register_factory<AudioSource, DragDropAudio>();
			register_factory<Prefab, DragDropPrefab>();
		}

		template <typename T, typename TFactory>
		void register_factory()
		{
			mFactories.emplace(rttr::type::get<T>(), CreateRef<TFactory>());
		}

		void register_factory(const rttr::type &type, const Ref<DragDropFactory> &factory) { mFactories.emplace(type, factory); }

		Ref<DragDropFactory> get_factory(const rttr::type &type)
		{
			if (mFactories.contains(type))
				return mFactories.at(type);

			return nullptr;
		}

	private:
		std::unordered_map<rttr::type, Ref<DragDropFactory>> mFactories;
	};

	Ref<GameObject> DragDropFactory::create_from(const Ref<Asset> &asset, const std::string &name, World *world, const FTransform &transform)
	{
		if (!asset)
		{
			return nullptr;
		}

		auto object = world->CreateGameObject(name);
		object->SetLocalTransform(transform);
		post_create(asset, object);

		return object;
	}

	Ref<DragDropFactory> DragDropFactory::get_factory_from_type(const rttr::type &type)
	{
		return get_registry().get_factory(type);
	}

	void DragDropFactory::register_factory(const rttr::type &type, const Ref<DragDropFactory> &factory)
	{
		get_registry().register_factory(type, factory);
	}

	DragDropFactoryRegistry &DragDropFactory::get_registry()
	{
		static DragDropFactoryRegistry registry;
		return registry;
	}

} // namespace BHive