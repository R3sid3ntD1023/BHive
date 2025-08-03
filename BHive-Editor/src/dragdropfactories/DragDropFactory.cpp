#include "DragDropFactory.h"
#include "asset/AssetManager.h"
#include "asset/EditorAssetManager.h"
#include "project/Project.h"
#include "world/GameObject.h"
#include "world/World.h"

namespace BHive
{
	struct DragDropFactoryRegistry
	{
		DragDropFactoryRegistry()
		{
			auto types = rttr::type::get<DragDropFactory>().get_derived_classes();
			for (auto &type : types)
			{
				if (!type.get_constructor())
				{
					LOG_WARN("No Default Constructor for {}", type);
					continue;
				}

				auto factory = type.create().get_value<Ref<DragDropFactory>>();
				mFactories.push_back(factory);
			}
		}

		Ref<DragDropFactory> GetFactory(const rttr::type &type)
		{
			auto it = std::find_if(mFactories.begin(), mFactories.end(), [=](const Ref<DragDropFactory> &factory) { return factory->CanCreateEntityFrom(type); });

			if (it != mFactories.end())
				return *it;

			return nullptr;
		}

	private:
		std::vector<Ref<DragDropFactory>> mFactories;
	};

	Ref<GameObject> DragDropFactory::CreateEntityFrom(const Ref<Asset> &asset, const std::string &name, World *world, const FTransform &transform)
	{
		if (!asset)
		{
			return nullptr;
		}

		auto object = world->CreateGameObject(name);
		object->SetLocalTransform(transform);
		PostCreateEntity(asset, object);

		return object;
	}

	Ref<DragDropFactory> DragDropFactory::GetDragDropFactory(const rttr::type &type)
	{

		static DragDropFactoryRegistry registry;
		return registry.GetFactory(type);
	}

} // namespace BHive