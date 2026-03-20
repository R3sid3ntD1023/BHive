#include "core/subsystem/SubSystem.h"
#include "GameObject.h"
#include "gfx/renderers/Renderer.h"
#include "systems/RenderSystem.h"
#include "systems/PhysicsSystem.h"
#include "World.h"

namespace BHive
{
	namespace callbacks
	{

	} // namespace callbacks

	void CopyComponents(const GameObject &src, GameObject &dst)
	{
		dst.GetComponents().clear();
		for (auto &component : src.GetComponents())
		{
			auto type = component->get_type();
			type.get_method(EMPLACE_OR_REPLACE_COMPONENT_FUNCTION_NAME).invoke(dst, component);
		}
	}

	World::World()
		: Asset()
	{
		mRenderSystem = CreateRef<RenderSystem>();
		mPhysicsSystem = CreateRef<PhysicsSystem>();
	}

	World::World(const World &world)
		: Asset(world)
	{
		mRenderSystem = CreateRef<RenderSystem>();
		mPhysicsSystem = CreateRef<PhysicsSystem>();
	}

	World::~World()
	{
	}

	void World::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(cereal::make_size_tag(mObjects.size()));

		for (auto &[id, obj] : mObjects)
		{
			ar(obj ? obj->get_type() : InvalidType);
			if (obj)
			{
				obj->Save(ar);
			}
		}
	}

	void World::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);

		size_t num_objects = 0;
		ar(cereal::make_size_tag(num_objects));

		for (size_t i = 0; i < num_objects; i++)
		{
			rttr::type obj_type = InvalidType;

			ar(obj_type);

			if (!obj_type)
				continue;

			auto obj = obj_type.create({mRegistry.create(), this}).get_value<Ref<GameObject>>();
			obj->Load(ar);
			AddGameObject(obj);
		}
	}

	void World::Begin()
	{
		SimulateBegin();

		for (auto &object : mObjects)
			object.second->Begin();

		mIsRunning = true;
	}

	void World::Update(float dt, SceneRenderer *renderer)
	{
		if (mIsRunning)
		{
			if (!mIsPaused || mFrames-- > 0)
			{
				Simulate(dt);

				for (auto &[id, object] : mObjects)
				{
					if (object->IsPendingDestroy())
					{
						object->End();
					}
					else
					{
						object->Update(dt);
					}
				}
			}
		}

		mRenderSystem->OnUpdate(renderer, this);
		mPhysicsSystem->DebugDraw();

		while (!mDestroyQueue.empty())
		{
			auto &identifier = mDestroyQueue.front();

			mObjects.erase(identifier.first);
			mEnttMap.erase(identifier.second);
			mRegistry.destroy(identifier.second);
			mDestroyQueue.pop();
		}
	}

	void World::End()
	{
		for (auto &object : mObjects)
			object.second->End();

		SimulateEnd();

		mIsRunning = false;
	}

	void World::SimulateBegin()
	{
		mPhysicsSystem->Init(this);
	}

	void World::Simulate(float dt)
	{
		mPhysicsSystem->Update(dt, this);
	}

	void World::SimulateEnd()
	{
		mPhysicsSystem->Shutdown(this);
	}

	void World::SetPaused(bool paused)
	{
		mIsPaused = paused;
	}

	void World::Step(int32_t frames)
	{
		mFrames = frames;
	}

	Ref<World> World::Copy() const
	{
		auto new_world = CreateRef<World>(*this);
		new_world->SetName("Instance");
		auto &dst_registry = new_world->mRegistry;

		auto &objects = GetGameObjects();

		for (auto &[id, src_obj] : objects)
		{
			auto type = src_obj->get_type();

			auto new_obj = type.create({dst_registry.create(), new_world.get()}).get_value<Ref<GameObject>>();

			if (!new_obj)
				continue;

			CopyComponents(*src_obj, *new_obj);
			new_world->AddGameObject(new_obj);
		}

		return new_world;
	}

	void World::Resize(uint32_t w, uint32_t h)
	{
		mRenderSystem->OnResize({w, h}, this);
	}

	Ref<GameObject> World::CreateGameObject(const rttr::type &type)
	{
		auto obj = type.create({mRegistry.create(), this}).get_value<Ref<GameObject>>();
		AddGameObject(obj);
		return obj;
	}

	void World::AddGameObject(const Ref<GameObject> &object)
	{
		if (!object)
			return;

		const auto &id = object->GetID();
		mObjects.emplace(id, object);
		mEnttMap.emplace(*object, id);

		LOG_TRACE("Added gameobject with ID: {}", (std::string)id);
	}

	GameObject *World::DuplicateGameobject(GameObject *object)
	{
		if (!object)
			return nullptr;

		auto obj = object->get_type().create({mRegistry.create(), this}).get_value<Ref<GameObject>>();

		for (auto &component : object->GetComponents())
		{
			const auto component_type = component->get_type();
			component_type.get_method(EMPLACE_OR_REPLACE_COMPONENT_FUNCTION_NAME).invoke(obj, component);
		}

		AddGameObject(obj);

		LOG_TRACE("Duplicated gameObject - {}", object->GetName());

		return obj.get();
	}

	Ref<GameObject> World::GetGameObject(const UUID &id) const
	{
		if (mObjects.contains(id))
			return mObjects.at(id);

		return nullptr;
	}

	Ref<GameObject> World::GetGameObject(int32_t id) const
	{
		auto entity = (entt::entity)(uint32_t)id;
		if (mEnttMap.contains(entity))
			return mObjects.at(mEnttMap.at(entity));

		return nullptr;
	}

	void World::Destroy(const UUID &gameObjectID)
	{
		if (!mObjects.contains(gameObjectID))
			return;

		entt::entity entity = *mObjects.at(gameObjectID);
		mDestroyQueue.push({gameObjectID, entity});
	}

	bool World::RayCast(const glm::vec3 &start, const glm::vec3 &dir, float maxDistance, FHitResult &result, uint16_t categoryMasks)
	{
		return mPhysicsSystem->RayCast(start, dir, maxDistance, result, categoryMasks);
	}

	REFLECT(World)
	{
		BEGIN_REFLECT(World) REFLECT_CONSTRUCTOR();
		rttr::type::register_wrapper_converter_for_base_classes<Ref<World>>();
	}
} // namespace BHive