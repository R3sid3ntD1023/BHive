#pragma once

#include "core/Core.h"
#include "gfx/Enumerations.h"
#include "Command.h"
#include <imgui.h>

#include "gfx/material/Material.h"
#include "gfx/material/ComputeBindings.h"

namespace BHive
{
	class Pipeline;
	class Material;
	class BufferBase;
	class VertexArray;

	struct CmdGenerateMipMaps : FCommand
	{
		Ref<Texture> TextureRef;

		void operator()(const Ref<Texture> &texture) { TextureRef = texture; }

		ECommandType GetType() const override { return ECommandType::GenerateMipMaps; }
	};

	struct CmdDisptach : FCommand
	{
		uint32_t X, Y, Z;

		void operator()(uint32_t x, uint32_t y, uint32_t z)
		{
			X = x;
			Y = y;
			Z = z;
		}

		ECommandType GetType() const override { return ECommandType::Dispatch; }
	};

	struct CmdImGuiRender : FCommand
	{
		ImDrawData *DrawData = nullptr;

		void operator()(ImDrawData *data) { DrawData = data; }

		ECommandType GetType() const override { return ECommandType::ImGuiRender; }
	};

	struct CmdDrawFullScreen : FCommand
	{
		void operator()() {}

		ECommandType GetType() const override { return ECommandType::DrawFullScreen; }
	};

	struct CmdBindPipeline : FCommand
	{
		Pipeline *PipelineRef;

		void operator()(Pipeline *pipeline) { PipelineRef = pipeline; }

		ECommandType GetType() const override { return ECommandType::BindPipeline; }
	};

	/*
	 * @param const IMaterial*, const Ref<IMaterial>&
	 * @param Pipeline*
	 */
	struct CmdBindMaterial : FCommand
	{
		MaterialSnapshot Snapshot;

		void operator()(IMaterial *mat) { Snapshot = mat->CreateSnapshot(); }

		ECommandType GetType() const override { return ECommandType::BindMaterial; }
	};

	struct CmdUploadBuffer : FCommand
	{
		BufferBase *Buffer;

		Ref<std::vector<std::byte>> Data;

		uint32_t Offset;

		void operator()(BufferBase *buffer, const void *data, size_t size, uint32_t offset = 0)
		{
			Buffer = buffer;
			Offset = offset;
			Data = CreateRef<std::vector<std::byte>>(size);
			memcpy(Data->data(), data, size);
		}

		ECommandType GetType() const override { return ECommandType::UploadBuffer; }
	};

	struct CmdDraw : FCommand
	{
		ETopologyMode Mode;

		Ref<VertexArray> VAO;

		uint32_t Count;

		void operator()(ETopologyMode mode, Ref<VertexArray> vao, uint32_t count)
		{
			Mode = mode;
			VAO = vao;
			Count = count;
		}

		ECommandType GetType() const override { return ECommandType::Draw; }
	};

	struct CmdDrawIndexed : FCommand
	{
		ETopologyMode Mode;

		Ref<VertexArray> VAO;

		uint32_t Count;

		void operator()(ETopologyMode mode, Ref<VertexArray> vao, uint32_t count = 0)
		{
			Mode = mode;
			VAO = vao;
			Count = count;
		}

		ECommandType GetType() const override { return ECommandType::DrawIndexed; }
	};

	struct CmdMultiDrawIndexedIndirect : FCommand
	{
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

		ECommandType GetType() const override { return ECommandType::MultiDrawIndexedIndirect; }
	};

	struct CmdSetLineWidth : FCommand
	{
		float Width = 1.f;

		void operator()(float width) { Width = width; }

		ECommandType GetType() const override { return ECommandType::SetLineWidth; }
	};
} // namespace BHive