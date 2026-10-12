#pragma once

#include <cstdint>

namespace BHive
{
	enum class ERenderFrameState : uint8_t
	{
		Idle,
		Recording,
		Submitted
	};

	struct FRenderFrame
	{
		float DeltaTime = 0.0f;
		uint64_t Index = 0;
		ERenderFrameState State = ERenderFrameState::Idle;
	};
} // namespace BHive
