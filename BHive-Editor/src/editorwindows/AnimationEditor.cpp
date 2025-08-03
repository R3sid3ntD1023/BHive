#include "AnimationEditor.h"
#include "mesh/Skeleton.h"

namespace BHive
{
	void AnimationEditor::OnWindowRender()
	{
		Inspect::get().inspect("Duration", mAsset, mAsset->GetDuration(), false);
		Inspect::get().inspect("TicksPerSecond", mAsset, mAsset->GetTicksPerSecond(), false);
		Inspect::get().inspect("Length (seconds)", mAsset, mAsset->GetLengthInSeconds(), false);
		Inspect::get().inspect("Skeleton", mAsset, mAsset->GetSkeleton(), false);
	}
} // namespace BHive