#pragma once

#include "Command.h"
#include "core/Core.h"
#include "gfx/Enumerations.h"
#include "Phase.h"
#include <imgui.h>

#include "gfx/material/Material.h"
#include "gfx/Query.h"

namespace BHive
{

	struct CmdGenerateMipMaps
	{
		static constexpr ECommandType Type = ECommandType::GenerateMipMaps;

		int32_t Texture;

		void operator()(int32_t texture) { Texture = texture; }
	};

	struct CmdClearBuffer
	{
		static constexpr ECommandType Type = ECommandType::ClearBuffer;

		BufferPtr Buffer;

		void operator()(BufferPtr b) { Buffer = b; }
	};

	struct CmdSetBufferData
	{
		static constexpr ECommandType Type = ECommandType::SetBufferData;

		BufferPtr Buffer;

		FCommandStream *Storage = nullptr;
		size_t DataOffset = 0;

		size_t Size = 0;

		uint32_t Offset = 0;

		void operator()(BufferPtr b, const void *data, size_t size, uint32_t offset = 0)
		{
			Buffer = b;
			Size = size;
			Offset = offset;
			DataOffset = Storage->StoreBytes(data, size);
		}
	};

	struct CmdDispatch
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

	struct CmdImGuiRender
	{
		static constexpr ECommandType Type = ECommandType::ImGuiRender;

		ImDrawData *DrawData = nullptr;

		void operator()(ImDrawData *data) { DrawData = data; }
	};

	struct CmdDrawFullScreen
	{
		static constexpr ECommandType Type = ECommandType::DrawFullScreen;

		void operator()() {}
	};

	struct CmdBindPipeline
	{
		static constexpr ECommandType Type = ECommandType::BindPipeline;

		PipelinePtr Pipeline;

		void operator()(PipelinePtr pipeline) { Pipeline = pipeline; }
	};

	struct CmdBindVertexArray
	{
		static constexpr ECommandType Type = ECommandType::BindVertexArray;

		VertexArrayPtr VertexArray;

		void operator()(VertexArrayPtr vertexArray) { VertexArray = vertexArray; }
	};

	/*
	 * @param const IMaterial*, const Ref<IMaterial>&
	 * @param Pipeline*
	 */
	struct CmdBindMaterial
	{
		static constexpr ECommandType Type = ECommandType::BindMaterial;

		FCommandStream *Storage = nullptr;
		uint32_t SnapshotIndex = 0;

		void operator()(const IMaterial *material, EShaderMeshType meshType = EShaderMeshType::None)
		{
			if (!material)
				return;

			SnapshotIndex = Storage->StoreObject<MaterialSnapshot>(material->CreateSnapshot(meshType));
		}

		void operator()(IMaterial *mat, EShaderMeshType meshType = EShaderMeshType::None) { SnapshotIndex = Storage->StoreObject<MaterialSnapshot>(mat->CreateSnapshot(meshType)); }
	};

	struct CmdUploadBuffer
	{
		static constexpr ECommandType Type = ECommandType::UploadBuffer;

		BufferPtr Buffer;

		FCommandStream *Storage = nullptr;
		size_t DataOffset = 0;
		size_t Size = 0;

		uint32_t Offset = 0;

		void operator()(BufferPtr buffer, const void *data, size_t size, uint32_t offset = 0)
		{
			Buffer = buffer;
			Offset = offset;
			Size = size;
			DataOffset = Storage->StoreBytes(data, size);
		}
	};

	struct CmdDraw
	{
		static constexpr ECommandType Type = ECommandType::Draw;

		ETopologyMode Mode;

		VertexArrayPtr VAO;

		uint32_t Count;

		void operator()(ETopologyMode mode, VertexArrayPtr vao, uint32_t count)
		{
			Mode = mode;
			VAO = vao;
			Count = count;
		}
	};

	struct CmdDrawIndexed
	{
		static constexpr ECommandType Type = ECommandType::DrawIndexed;

		ETopologyMode Mode;

		VertexArrayPtr VAO;

		uint32_t Count;

		void operator()(ETopologyMode mode, VertexArrayPtr vao, uint32_t count = 0)
		{
			Mode = mode;
			VAO = vao;
			Count = count;
		}
	};

	struct CmdMultiDrawIndexedIndirect
	{
		static constexpr ECommandType Type = ECommandType::MultiDrawIndexedIndirect;

		ETopologyMode Mode;
		BufferPtr Buffer;
		uint32_t DrawCount = 1;
		uint64_t Stride = 0;
		uint32_t Offset = 0;

		void operator()(ETopologyMode mode, BufferPtr buffer, uint32_t drawCount, uint64_t stride = 0, uint32_t offset = 0)
		{
			Mode = mode;
			Buffer = buffer;
			DrawCount = drawCount;
			Stride = stride;
			Offset = offset;
		}
	};

	struct CmdSetLineWidth
	{
		static constexpr ECommandType Type = ECommandType::SetLineWidth;

		float Width = 1.f;

		void operator()(float width) { Width = width; }
	};
} // namespace BHive