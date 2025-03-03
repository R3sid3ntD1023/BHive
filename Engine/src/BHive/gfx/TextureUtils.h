#pragma once

#include "Texture.h"

namespace BHive
{
	extern uint32_t GetTextureTarget(ETextureType type, uint32_t samples = 1);

	uint32_t GetTextureCompareMode(ETextureCompareMode mode);

	uint32_t GetTextureCompareFunc(ETextureCompareFunc func);

	uint32_t GetTextureFormat(ETextureFormat format);

	uint32_t GetGLFilterMode(EMinFilter mode);
	uint32_t GetGLFilterMode(EMagFilter mode);
	uint32_t GetGLWrapMode(EWrapMode mode);
	uint32_t GetGLFormat(EFormat format);
	uint32_t GetGLInternalFormat(EFormat format);
	uint32_t GetGLType(EFormat format);
	EFormat GetFormat(uint32_t channels);
	bool IsDepthFormat(EFormat format);

	uint32_t GetDepthAttachmentType(EFormat format);

	EFormat GetFormatFromChannels(bool hdr, int channels);

	uint32_t GetGLAccess(EImageAccess access);
} // namespace BHive