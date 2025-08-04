#include "Animator/anim_graph/AnimGraph.h"
#include "AnimGraphFactory.h"
#include "inspectors/Inspect.h"
#include "mesh/Skeleton.h"
#include "core/subsystem/SubSystem.h"
#include "windows/ImWindowSystem.h"
#include "windows/ImWindowBase.h"

namespace BHive
{
	struct AnimGraphOptions : public ImWindowBase
	{
		AnimGraphOptions(AnimGraphFactory *factory)
			: ImWindowBase(ImGuiWindowFlags_NoSavedSettings),
			  mFactory(factory)
		{
		}

		virtual void OnUpdate() final override
		{
			Inspect::get().inspect("Skeleton", this, mSkeleton);

			if (mSkeleton)
			{
				if (ImGui::Button("Create"))
				{
					mFactory->OnAssetCreated.invoke(CreateRef<AnimGraph>(mSkeleton));
					mShouldClose = true;
				}
			}
		}

		bool ShouldClose() const override { return ImWindowBase::ShouldClose() || mShouldClose; };

	private:
		AnimGraphFactory *mFactory;
		bool mShouldClose{false};
		Ref<Skeleton> mSkeleton;
	};

	Ref<Asset> AnimGraphFactory::CreateNew()
	{
		auto &window_system = SubSystemContext::Get().GetSubSystem<ImWindowSystem>();
		auto window = window_system.ConstructWindow<AnimGraphOptions>(this);

		return nullptr;
	}

	REFLECT_FACTORY(AnimGraphFactory);
} // namespace BHive