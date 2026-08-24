#pragma once

namespace BHive
{
	enum class ECommandType : uint8_t
	{
		GenerateMipMaps,
		BindPipeline,
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

	struct FCommand
	{
		virtual ~FCommand() = default;
	};
} // namespace BHive