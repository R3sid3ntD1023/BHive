#include "AnimationEditor.h"
#include "inspector/Inspectors.h"
#include "mesh/Skeleton.h"

namespace BHive
{
	void AnimationEditor::OnWindowRender()
	{
		inspect("Duration", mAsset->GetDuration(), false);
		inspect("TicksPerSecond", mAsset->GetTicksPerSecond(), false);
		inspect("Length (seconds)", mAsset->GetLengthInSeconds(), false);
		inspect("Skeleton", TAssetHandle<Skeleton>(mAsset->GetSkeleton()), false);
	}
} // namespace BHive