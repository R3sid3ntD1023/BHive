#include "Animator/anim_graph/AnimGraph.h"
#include "AnimGraphFactory.h"
#include "inspectors/Inspect.h"
#include "mesh/Skeleton.h"
#include "core/subsystem/SubSystem.h"
#include "subsystems/WindowSubSystem.h"
#include "WindowBase.h"

namespace BHive
{
	struct AnimGraphOptions : public WindowBase
	{
		AnimGraphOptions(AnimGraphFactory *factory)
			: WindowBase(ImGuiWindowFlags_NoSavedSettings),
			  mFactory(factory)
		{
		}

		virtual void OnGuiRender() final override
		{
			Inspect::inspect("Skeleton", mSkeleton);

			if (mSkeleton)
			{
				if (ImGui::Button("Create"))
				{
					mFactory->OnAssetCreated.invoke(CreateRef<AnimGraph>(mSkeleton));
					mShouldClose = true;
				}
			}
		}

		bool ShouldClose() const override { return WindowBase::ShouldClose() || mShouldClose; };

	private:
		AnimGraphFactory *mFactory;
		bool mShouldClose{false};
		Ref<Skeleton> mSkeleton;
	};

	Ref<Asset> AnimGraphFactory::CreateNew()
	{
		auto &window_system = SubSystemContext::Get().GetSubSystem<WindowSubSystem>();
		auto window = window_system.CreateWindow<AnimGraphOptions>(this);

		return nullptr;
	}

	REFLECT_FACTORY(AnimGraphFactory);
} // namespace BHive