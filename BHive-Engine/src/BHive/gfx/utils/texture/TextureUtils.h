#pragma once

#include "gfx/TextureSpecification.h"

namespace BHive
{
	struct TextureUtils
	{
		static uint32_t GetAPITextureCompareMode(ETextureCompareMode mode);

		static uint32_t GetAPITextureCompareFunc(ETextureCompareFunc func);

		static uint32_t GetTextureFormat(ETextureFormat format);

		static uint32_t GetAPIFilterMode(EMinFilter mode);

		static uint32_t GetAPIFilterMode(EMagFilter mode);

		static uint32_t GetAPIWrapMode(EWrapMode mode);

		static uint32_t GetAPIFormat(EFormat format);

		static uint32_t GetAPIInternalFormat(EFormat format);

		static uint32_t GetAPIType(EFormat format);

		static bool IsDepthFormat(EFormat format);

		static uint32_t GetAPIDepthAttachmentType(EFormat format);

		static uint32_t GetAPIImageAccess(EImageAccess access);
	};
} // namespace BHive