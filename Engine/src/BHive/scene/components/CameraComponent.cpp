#include "CameraComponent.h"
#include "core/Time.h"
#include "math/Frustum.h"
#include "renderers/LineRenderer.h"

namespace BHive
{
	void CameraComponent::OnRender(SceneRenderer *renderer)
	{
		auto frustrum = Frustum(mCamera.GetProjection(), GetWorldTransform().inverse());
		LineRenderer::DrawFrustum(frustrum, 0xFF00FFFF);
	}

	void CameraComponent::SetPrimary(bool primary)
	{
		mIsPrimary = primary;
	}

	bool CameraComponent::IsPrimary() const
	{
		return mIsPrimary;
	}

	void CameraComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		SceneComponent::Save(ar);

		ar(mIsPrimary, mCamera);
	}

	void CameraComponent::Load(cereal::BinaryInputArchive &ar)
	{
		SceneComponent::Load(ar);

		ar(mIsPrimary, mCamera);
	}

	void CameraManager::SetTargetViewWithBlend(const FTransform &view, float blendtime)
	{
		float last_time = 0.0f;
		float current_time = blendtime;
		FTransform current_view = sCurrentViewTarget;

		while (current_time > 0.0f)
		{
			float time = Time::Get();
			float deltatime = time - last_time;
			last_time = time;

			current_time -= deltatime;
			sCurrentViewTarget = Math::Lerp(current_view, view, current_time);
		}
	}

	void CameraManager::SetTargetView(const FTransform &view)
	{
		sCurrentViewTarget = view;
	}

	const FTransform &CameraManager::GetTargetView()
	{
		return sCurrentViewTarget;
	}

} // namespace BHive