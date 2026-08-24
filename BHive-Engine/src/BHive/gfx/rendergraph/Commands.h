#pragma once

#include "core/Core.h"
#include "gfx/Enumerations.h"
#include "Command.h"
#include <imgui.h>

#include "gfx/material/Material.h"
#include "gfx/material/ComputeBindings.h"
#include "gfx/Query.h"

namespace BHive
{
	class Pipeline;
	class Material;
	class BufferBase;
	class VertexArray;

	struct CmdGenerateMipMaps : FCommand
	{
		static constexpr ECommandType Type = ECommandType::GenerateMipMaps;

		Ref<Texture> TextureRef;

		void operator()(const Ref<Texture> &texture) { TextureRef = texture; }
	};

	struct CmdClearBuffer : FCommand
	{
		static constexpr ECommandType Type = ECommandType::ClearBuffer;

		Ref<BufferBase> BufferRef;

		void operator()(const Ref<BufferBase> &b) { BufferRef = b; }
	};

	struct CmdSetBufferData : FCommand
	{
		static constexpr ECommandType Type = ECommandType::SetBufferData;

		Ref<BufferBase> BufferRef;

		std::vector<std::byte> Data;

		uint32_t Size;

		uint32_t Offset;

		void operator()(const Ref<BufferBase> &b, const void *data, size_t size, uint32_t offset = 0)
		{
			BufferRef = b;
			Size = size;
			Offset = offset;
			Data.resize(size);
			std::memcpy(Data.data(), data, size);
		}
	};

	struct CmdDispatch : FCommand
	{
		static constexpr ECommandType Type = ECommandType::Dispatch;

		uint32_t X, Y, Z;

		void operator()(uint32_t x, uint32_t y, uint32_t z)
		{
			X = x;
			Y = y;
			Z = z;
		}
	};

	struct CmdImGuiRender : FCommand
	{
		static constexpr ECommandType Type = ECommandType::ImGuiRender;

		ImDrawData *DrawData = nullptr;

		void operator()(ImDrawData *data) { DrawData = data; }
	};

	struct CmdDrawFullScreen : FCommand
	{
		static constexpr ECommandType Type = ECommandType::DrawFullScreen;

		void operator()() {}
	};

	struct CmdBindPipeline : FCommand
	{
		static constexpr ECommandType Type = ECommandType::BindPipeline;

		Pipeline *PipelineRef;

		void operator()(Pipeline *pipeline) { PipelineRef = pipeline; }
	};

	/*
	 * @param const IMaterial*, const Ref<IMaterial>&
	 * @param Pipeline*
	 */
	struct CmdBindMaterial : FCommand
	{
		static constexpr ECommandType Type = ECommandType::BindMaterial;

		MaterialSnapshot Snapshot;

		void operator()(IMaterial *mat) { Snapshot = mat->CreateSnapshot(); }
	};

	struct CmdUploadBuffer : FCommand
	{
		static constexpr ECommandType Type = ECommandType::UploadBuffer;

		Ref<BufferBase> Buffer;

		Ref<std::vector<std::byte>> Data;

		uint32_t Offset;

		void operator()(const Ref<BufferBase> &buffer, const void *data, size_t size, uint32_t offset = 0)
		{
			Buffer = buffer;
			Offset = offset;
			Data = CreateRef<std::vector<std::byte>>(size);
			memcpy(Data->data(), data, size);
		}
	};

	struct CmdDraw : FCommand
	{
		static constexpr ECommandType Type = ECommandType::Draw;

		ETopologyMode Mode;

		Ref<VertexArray> VAO;

		uint32_t Count;

		void operator()(ETopologyMode mode, Ref<VertexArray> vao, uint32_t count)
		{
			Mode = mode;
			VAO = vao;
			Count = count;
		}
	};

	struct CmdDrawIndexed : FCommand
	{
		static constexpr ECommandType Type = ECommandType::DrawIndexed;

		ETopologyMode Mode;

		Ref<VertexArray> VAO;

		uint32_t Count;

		void operator()(ETopologyMode mode, Ref<VertexArray> vao, uint32_t count = 0)
		{
			Mode = mode;
			VAO = vao;
			Count = count;
		}
	};

	struct CmdMultiDrawIndexedIndirect : FCommand
	{
		static constexpr ECommandType Type = ECommandType::MultiDrawIndexedIndirect;

		ETopologyMode Mode;
		BufferBase *Buffer;
		VertexArray *VAO;
		uint32_t DrawCount = 1;
		uint64_t Stride = 0;
		uint32_t Offset = 0;

		void operator()(ETopologyMode mode, BufferBase *buffer, VertexArray *vao, uint32_t drawCount, uint64_t stride = 0, uint32_t offset = 0)
		{
			Mode = mode;
			Buffer = buffer;
			VAO = vao;
			DrawCount = drawCount;
			Stride = stride;
			Offset = offset;
		}
	};

	struct CmdSetLineWidth : FCommand
	{
		static constexpr ECommandType Type = ECommandType::SetLineWidth;

		float Width = 1.f;

		void operator()(float width) { Width = width; }
	};
} // namespace BHive