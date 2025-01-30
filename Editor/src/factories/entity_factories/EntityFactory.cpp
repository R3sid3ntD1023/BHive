#include "EntityFactory.h"
#include "scene/World.h"
#include "scene/Entity.h"
#include "asset/AssetManager.h"
#include "asset/EditorAssetManager.h"

namespace BHive
{
	Ref<Entity> EntityFactory::CreateEntityFrom(const FAssetMetaData &metaData, World *world, const FTransform &transform)
	{
		auto entity = world->CreateEntity(metaData.Name);
		entity->SetLocalTransform(transform);

		auto manager = AssetManager::GetAssetManager<EditorAssetManager>();
		if (manager)
		{
			auto handle = manager->GetHandle(metaData.Path);
			auto asset = manager->GetAsset(handle);
			if (handle && asset)
			{
				PostCreateEntity(asset, entity);
			}
		}

		return entity;
	}

	struct EntityFactoryRegistry
	{
		EntityFactoryRegistry()
		{
			auto types = AssetType::get<EntityFactory>().get_derived_classes();
			for (auto &type : types)
			{
				auto factory = type.create().get_value<Ref<EntityFactory>>();
				mFactories.push_back(factory);
			}
		}

		Ref<EntityFactory> GetFactory(const FAssetMetaData &metaData)
		{
			auto it = std::find_if(
				mFactories.begin(), mFactories.end(),
				[metaData](const Ref<EntityFactory> &factory) { return factory->CanCreateEntityFrom(metaData); });

			if (it != mFactories.end())
				return *it;

			return nullptr;
		}

	private:
		std::vector<Ref<EntityFactory>> mFactories;
	};

	Ref<EntityFactory> GetEntityFactory(const FAssetMetaData &metaData)
	{
		static EntityFactoryRegistry registry;
		return registry.GetFactory(metaData);
	}
} // namespace BHive