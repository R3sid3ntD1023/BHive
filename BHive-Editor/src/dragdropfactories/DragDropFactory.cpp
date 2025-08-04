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
			RegisterFactory<StaticMesh, DragDropStaticMesh>();
			RegisterFactory<SkeletalMesh, DragDropSkeletalMesh>();
			RegisterFactory<AudioSource, DragDropAudio>();
			RegisterFactory<Prefab, DragDropPrefab>();
		}

		template <typename T, typename TFactory>
		void RegisterFactory()
		{
			mFactories.emplace(rttr::type::get<T>(), CreateRef<TFactory>());
		}

		void RegisterFactory(const rttr::type &type, const Ref<DragDropFactory> &factory) { mFactories.emplace(type, factory); }

		Ref<DragDropFactory> GetFactory(const rttr::type &type)
		{
			if (mFactories.contains(type))
				return mFactories.at(type);

			return nullptr;
		}

	private:
		std::unordered_map<rttr::type, Ref<DragDropFactory>> mFactories;
	};

	Ref<GameObject> DragDropFactory::CreateFrom(const Ref<Asset> &asset, const std::string &name, World *world, const FTransform &transform)
	{
		if (!asset)
		{
			return nullptr;
		}

		auto object = world->CreateGameObject(name);
		object->SetLocalTransform(transform);
		PostCreate(asset, object);

		return object;
	}

	Ref<DragDropFactory> DragDropFactory::GetFactoryFromType(const rttr::type &type)
	{
		return GetRegistry().GetFactory(type);
	}

	void DragDropFactory::RegisterFactory(const rttr::type &type, const Ref<DragDropFactory> &factory)
	{
		GetRegistry().RegisterFactory(type, factory);
	}

	DragDropFactoryRegistry &DragDropFactory::GetRegistry()
	{
		static DragDropFactoryRegistry registry;
		return registry;
	}

} // namespace BHive