#pragma once

#include "ImageInfo.h"
#include "Command.h"
#include "GlobalBinding.h"

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

	struct FBufferUsageInfo
	{
		Ref<BufferBase> Buffer;
		EBufferUsage Access;
	};

	struct FBufferTransition
	{
		Ref<BufferBase> Buffer;
		EBufferUsage Src;
		EBufferUsage Dst;
	};

	struct CmdHeader
	{
		ECommandType Type;
		size_t Size = 0;
	};

	struct FPhase
	{
		std::string Name;

		EPhaseType Type = EPhaseType::Graphics;

		Ref<Framebuffer> FBO;

		std::vector<FImageInfo> Images;

		std::vector<FBufferUsageInfo> Buffers;

		std::vector<FBufferTransition> BufferTransitions;

		ImageSubresourceRange ColorRange{};

		std::unordered_map<GlobalBinding, Ref<BufferBase>> BoundBuffers;

		std::unordered_map<GlobalBinding, Ref<Texture>> BoundTextures;

		std::vector<std::pair<CmdHeader, Ref<FCommand>>> Commands;

		// std::vector<std::byte> CommandPtr;
	};

} // namespace BHive
