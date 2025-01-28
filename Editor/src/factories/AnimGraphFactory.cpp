#include "Animator/anim_graph/AnimGraph.h"
#include "AnimGraphFactory.h"
#include "inspector/Inspectors.h"
#include "mesh/Skeleton.h"
#include "subsystem/SubSystem.h"
#include "subsystems/WindowSubSystem.h"
#include "windows/WindowBase.h"

namespace BHive
{
	struct AnimGraphOptions : public WindowBase
	{
		AnimGraphOptions(AnimGraphFactory *factory)
			: WindowBase(ImGuiWindowFlags_NoSavedSettings),
			  mFactory(factory)
		{
		}

		virtual void OnUpdateContent() final override
		{
			inspect("Skeleton", mSkeleton);

			if (mSkeleton)
			{
				if (ImGui::Button("Create"))
				{
					mFactory->OnAssetCreated.invoke(CreateRef<AnimGraph>(mSkeleton.get()));
					mShouldClose = true;
				}
			}
		}

		bool ShouldClose() const override { return WindowBase::ShouldClose() || mShouldClose; };

	private:
		AnimGraphFactory *mFactory;
		bool mShouldClose{false};
		TAssetHandle<Skeleton> mSkeleton;
	};

	Ref<Asset> AnimGraphFactory::CreateNew()
	{
		auto &window_system = SubSystemContext::Get().GetSubSystem<WindowSubSystem>();
		auto window = window_system.AddWindow<AnimGraphOptions>(this);

		return nullptr;
	}

	REFLECT_FACTORY(AnimGraphFactory);
} // namespace BHive