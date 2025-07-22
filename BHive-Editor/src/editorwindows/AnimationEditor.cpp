#include "AnimationEditor.h"
#include "mesh/Skeleton.h"

namespace BHive
{
	void AnimationEditor::OnWindowRender()
	{
		Inspect::inspect("Duration", mAsset->GetDuration(), false);
		Inspect::inspect("TicksPerSecond", mAsset->GetTicksPerSecond(), false);
		Inspect::inspect("Length (seconds)", mAsset->GetLengthInSeconds(), false);
		Inspect::inspect("Skeleton", mAsset->GetSkeleton(), false);
	}
} // namespace BHive