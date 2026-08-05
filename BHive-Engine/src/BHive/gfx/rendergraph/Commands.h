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

	/*
	 * @param r
	 * @param g
	 * @param b
	 * @param a
	 */
	struct CmdSetClearColor : FCommand
	{
		float R, G, B, A;

		CmdSetClearColor(float r, float g, float b, float a)
			: R(r),
			  G(g),
			  B(b),
			  A(a)
		{
		}

		ECommandType GetType() const override { return ECommandType::SetClearColor; }
	};

	/**/
	struct CmdClear : FCommand
	{
		CmdClear() {};

		ECommandType GetType() const override { return ECommandType::Clear; }
	};

	/*
	 * @param X
	 * @param Y
	 * @param Width
	 * @param height
	 */
	struct CmdSetViewport : FCommand
	{
		int32_t X, Y;
		uint32_t Width, Height;

		CmdSetViewport(int32_t x, int32_t y, uint32_t w, uint32_t h)
			: X(x),
			  Y(y),
			  Width(w),
			  Height(h)
		{
		}

		ECommandType GetType() const override { return ECommandType::SetViewport; }
	};

	struct CmdGenerateMipMaps : FCommand
	{
		Ref<Texture> Tex;

		CmdGenerateMipMaps(const Ref<Texture> &texture)
			: Tex(texture)
		{
		}

		ECommandType GetType() const override { return ECommandType::GenerateMipMaps; }
	};

	struct CmdDisptach : FCommand
	{
		uint32_t X, Y, Z;

		CmdDisptach(uint32_t x, uint32_t y, uint32_t z)
			: X(x),
			  Y(y),
			  Z(z)
		{
		}

		ECommandType GetType() const override { return ECommandType::Dispatch; }
	};

	struct CmdImGuiRender : FCommand
	{
		ImDrawData *DrawData;

		CmdImGuiRender(ImDrawData *data)
			: DrawData(data)
		{
		}

		ECommandType GetType() const override { return ECommandType::ImGuiRender; }
	};

	struct CmdDrawFullScreen : FCommand
	{
		CmdDrawFullScreen() {}

		ECommandType GetType() const override { return ECommandType::DrawFullScreen; }
	};

	struct CmdBindPipeline : FCommand
	{
		Pipeline *PipelineRef;

		CmdBindPipeline(Pipeline *pipeline)
			: PipelineRef(pipeline)
		{
		}

		ECommandType GetType() const override { return ECommandType::BindPipeline; }
	};

	/*
	 * @param const IMaterial*, const Ref<IMaterial>&
	 * @param Pipeline*
	 */
	struct CmdBindMaterial : FCommand
	{
		MaterialSnapshot Snapshot;

		bool BreakPoint = false;

		CmdBindMaterial(const IMaterial *mat, bool breakPoint = false)
			: Snapshot(mat->CreateSnapshot()),
			  BreakPoint(breakPoint)
		{
		}

		ECommandType GetType() const override { return ECommandType::BindMaterial; }
	};

	struct CmdSetGlobalTopology : FCommand
	{
		ETopologyMode Mode;

		CmdSetGlobalTopology(ETopologyMode mode)
			: Mode(mode)
		{
		}

		ECommandType GetType() const override { return ECommandType::SetGlobalTopology; }
	};

	struct CmdUploadBuffer : FCommand
	{
		BufferBase *Buffer;

		Ref<std::vector<std::byte>> Data;

		uint32_t Offset;

		CmdUploadBuffer(BufferBase *buffer, const void *data, size_t size, uint32_t offset = 0)
			: Buffer(buffer),
			  Offset(offset)

		{
			Data = CreateRef<std::vector<std::byte>>(size);
			memcpy(Data->data(), data, size);
		}

		ECommandType GetType() const override { return ECommandType::UploadBuffer; }
	};

	struct CmdDraw : FCommand
	{
		ETopologyMode Mode;

		VertexArray *VAO;

		uint32_t Count;

		CmdDraw(ETopologyMode mode, VertexArray *vao, uint32_t count)
			: Mode(mode),
			  VAO(vao),
			  Count(count)
		{
		}

		ECommandType GetType() const override { return ECommandType::Draw; }
	};

	struct CmdDrawIndexed : FCommand
	{
		ETopologyMode Mode;

		VertexArray *VAO;

		uint32_t Count;

		CmdDrawIndexed(ETopologyMode mode, VertexArray *vao, uint32_t count)
			: Mode(mode),
			  VAO(vao),
			  Count(count)
		{
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

		CmdMultiDrawIndexedIndirect(ETopologyMode mode, BufferBase *buffer, VertexArray *vao, uint32_t drawCount, uint64_t stride = 0, uint32_t offset = 0)
			: Mode(mode),
			  Buffer(buffer),
			  VAO(vao),
			  DrawCount(drawCount),
			  Stride(stride),
			  Offset(offset)
		{
		}

		ECommandType GetType() const override { return ECommandType::MultiDrawIndexedIndirect; }
	};

	struct CmdSetLineWidth : FCommand
	{
		float Width = 1.f;

		CmdSetLineWidth(float width)
			: Width(width)
		{
		}

		ECommandType GetType() const override { return ECommandType::SetLineWidth; }
	};
} // namespace BHive