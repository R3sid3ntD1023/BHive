#pragma once

namespace BHive
{
	enum class ECommandType : uint8_t
	{
		GenerateMipMaps,
		BindPipeline,
		BindVertexArray,
		BindMaterial,
		UploadBuffer,
		Draw,
		DrawIndexed,
		MultiDrawIndexedIndirect,
		SetLineWidth,
		Dispatch,
		ImGuiRender,
		DrawFullScreen,

		ClearBuffer,
		SetBufferData
	};

} // namespace BHive