#pragma once

#include "ImageInfo.h"
#include "RenderCommandList.h"

namespace BHive
{
	class Framebuffer;
	class BufferBase;

	enum class EPhaseType
	{
		Graphics,
		Compute,
		Transfer
	};

	struct FPhase
	{
		std::string Name;

		EPhaseType Type = EPhaseType::Graphics;

		FRenderCommandList CommandList;

		std::vector<FImageInfo> Images;

		std::vector<FBufferUse> Buffers;

		Ref<Framebuffer> FBO;

		/*FPhase() = default;
		~FPhase() = default;

		FPhase(const FPhase &) = delete;
		FPhase& operator=(const FPhase &) = delete;

		FPhase(FPhase &&) noexcept = default;
		FPhase &operator=(FPhase &&) noexcept = default;*/
	};

}