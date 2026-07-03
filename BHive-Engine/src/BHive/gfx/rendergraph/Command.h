#pragma once

namespace BHive
{
	enum class ECommandType
	{
		SetClearColor,
		Clear,
		SetViewport,
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
		SetGlobalTopology
	};

	struct FCommand
	{
		virtual ~FCommand() = default;

		virtual ECommandType GetType() const = 0;
	};
}