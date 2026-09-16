#include "AnimationEditor.h"

namespace BHive
{
	void AnimationEditor::OnUpdateContent()
	{
		Inspect::get().inspect("Duration", mAsset, mAsset->GetDuration());
		Inspect::get().inspect("TicksPerSecond", mAsset, mAsset->GetTicksPerSecond());
		Inspect::get().inspect("Length (seconds)", mAsset, mAsset->GetLengthInSeconds());
	}
} // namespace BHive