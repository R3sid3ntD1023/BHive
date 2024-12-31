#pragma once

#include "SceneComponent.h"
#include "IRenderable.h"
#include "scene/SceneCamera.h"

namespace BHive
{
	struct BHIVE CameraComponent : public SceneComponent, public IRenderable
	{
		SceneCamera mCamera;

		virtual void OnRender(SceneRenderer *renderer) override;

		void SetPrimary(bool primary);
		bool IsPrimary() const;

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

	private:
		bool mIsPrimary;

		REFLECTABLEV(SceneComponent)
	};

	REFLECT(CameraComponent)
	{
		BEGIN_REFLECT(CameraComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true))
			REQUIRED_COMPONENT_FUNCS()
				REFLECT_PROPERTY("Camera", mCamera)
					REFLECT_PROPERTY("Primary", IsPrimary, SetPrimary);
	}

	class CameraManager
	{
	public:
		static void SetTargetView(const FTransform &view);

		static void SetTargetViewWithBlend(const FTransform &view, float blendtime = 1.0f);

		static const FTransform &GetTargetView();

	private:
		static inline FTransform sCurrentViewTarget{1.0f};
	};
}
