#pragma once

#include "core/Core.h"
#include "runtime/ITickable.h"
#include "core/math/Transform.h"

namespace BHive
{
	class GameObject;
	class Renderer;
	class SceneRenderer;

	struct FComponentRenderContext
	{
		SceneRenderer &Scene;
		Renderer &EngineRenderer;
		int32_t EntityID;
		bool IsDirty;
	};

	struct BHIVE_API Component
	{
		virtual ~Component() = default;

		virtual void Begin() {};

		virtual void End() {};

		virtual void Save(cereal::BinaryOutputArchive &ar) const;

		virtual void Load(cereal::BinaryInputArchive &ar);

		virtual void OnRender(const FComponentRenderContext &context) {}

		void SetOwner(GameObject *owner);

		GameObject *GetOwner() const { return mOwningObject; }

		FTransform GetWorldTransform() const;

		bool IsDirty() const { return mIsDirty; }

		void MarkDirty() { mIsDirty = true; }

		void ClearDirty() { mIsDirty = false; }

		REFLECTABLEV()

	private:
		GameObject *mOwningObject = nullptr;
		bool mIsDirty = true;

		friend GameObject;
	};

	REFLECT_INLINE(Component)
	{
		BEGIN_REFLECT(Component);
	}

} // namespace BHive