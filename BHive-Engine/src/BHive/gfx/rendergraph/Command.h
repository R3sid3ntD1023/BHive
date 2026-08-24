#pragma once

namespace BHive
{
	enum class ECommandType
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

		virtual ECommandType GetType() const = 0;
	};
} // namespace BHive