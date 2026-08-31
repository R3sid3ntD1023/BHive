#pragma once

#include "Command.h"
#include "GlobalBinding.h"
#include "gfx/resources/ImageSubResourceRange.h"
#include "gfx/Enumerations.h";
#include "gfx/registries/Handles.h"

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

	// Tex, Mip, Levels, Layer, Layers, Access
	struct FImageInfo
	{
		TexturePtr Texture;
		EImageUsage Access = EImageUsage::None;
		ImageSubresourceRange Range{};
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

		FramebufferPtr FBO;

		std::vector<FImageInfo> Images;

		std::vector<FBufferUsageInfo> Buffers;

		std::vector<FBufferTransition> BufferTransitions;

		ImageSubresourceRange ColorRange{};

		std::unordered_map<GlobalBinding, Ref<BufferBase>> BoundBuffers;

		std::unordered_map<GlobalBinding, TexturePtr> BoundTextures;

		std::vector<std::pair<CmdHeader, Ref<FCommand>>> Commands;

		// std::vector<std::byte> CommandPtr;
	};

} // namespace BHive
