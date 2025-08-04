#include "AnimationEditor.h"
#include "mesh/Skeleton.h"

namespace BHive
{
	void AnimationEditor::OnContentUpdate()
	{
		Inspect::get().inspect("Duration", mAsset, mAsset->GetDuration());
		Inspect::get().inspect("TicksPerSecond", mAsset, mAsset->GetTicksPerSecond());
		Inspect::get().inspect("Length (seconds)", mAsset, mAsset->GetLengthInSeconds());
		Inspect::get().inspect("Skeleton", mAsset, mAsset->GetSkeleton());
	}
} // namespace BHive