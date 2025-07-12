#include "TextureUtils.h"
#include <glad/glad.h>

namespace BHive
{

	uint32_t GetTextureCompareMode(ETextureCompareMode mode)
	{
		switch (mode)
		{
		case BHive::ETextureCompareMode::NONE:
			return GL_NONE;
		case BHive::ETextureCompareMode::COMPARE_REF_TO_TEXTURE:
			return GL_COMPARE_REF_TO_TEXTURE;
		default:
			break;
		}
		ASSERT(false);
		return 0;
	}

	uint32_t GetTextureCompareFunc(ETextureCompareFunc func)
	{
		switch (func)
		{
		case BHive::ETextureCompareFunc::LEQUAL:
			return GL_LEQUAL;
		case BHive::ETextureCompareFunc::GEQUAL:
			return GL_GEQUAL;
		case BHive::ETextureCompareFunc::LESS:
			return GL_LESS;
		case BHive::ETextureCompareFunc::GREATER:
			return GL_GREATER;
		case BHive::ETextureCompareFunc::EQUAL:
			return GL_EQUAL;
		case BHive::ETextureCompareFunc::NOTEQUAL:
			return GL_NOTEQUAL;
		case BHive::ETextureCompareFunc::ALWAYS:
			return GL_ALWAYS;
		case BHive::ETextureCompareFunc::NEVER:
			return GL_NEVER;
		default:
			break;
		}
		ASSERT(false);
		return 0;
	}

	uint32_t GetTextureFormat(ETextureFormat format)
	{
		switch (format)
		{
		case BHive::ETextureFormat::R:
			return GL_RED;
		case BHive::ETextureFormat::RG:
			return GL_RG;
		case BHive::ETextureFormat::RGB:
			return GL_RGB;
		case BHive::ETextureFormat::RGBA:
			return GL_RGBA;
		default:
			break;
		}

		ASSERT(false);
		return 0;
	}

	uint32_t GetGLFilterMode(EMinFilter mode)
	{
		switch (mode)
		{
		case BHive::EMinFilter::LINEAR:
			return GL_LINEAR;
		case BHive::EMinFilter::NEAREST:
			return GL_NEAREST;
		case BHive::EMinFilter::MIPMAP_LINEAR:
			return GL_LINEAR_MIPMAP_LINEAR;
		case BHive::EMinFilter::MIPMAP_NEAREST:
			return GL_NEAREST_MIPMAP_NEAREST;
		case BHive::EMinFilter::MIPMAP_LINEAR_NEAREST:
			return GL_LINEAR_MIPMAP_NEAREST;
		case BHive::EMinFilter::MIPMAP_NEAREST_LINEAR:
			return GL_NEAREST_MIPMAP_LINEAR;
		default:
			break;
		}

		ASSERT(false);
		return 0;
	}

	uint32_t GetGLFilterMode(EMagFilter mode)
	{
		switch (mode)
		{
		case BHive::EMagFilter::LINEAR:
			return GL_LINEAR;
		case BHive::EMagFilter::NEAREST:
			return GL_NEAREST;
		default:
			break;
		}

		ASSERT(false);
		return 0;
	}

	uint32_t GetGLWrapMode(EWrapMode mode)
	{
		switch (mode)
		{
		case BHive::EWrapMode::REPEAT:
			return GL_REPEAT;
		case BHive::EWrapMode::CLAMP_TO_EDGE:
			return GL_CLAMP_TO_EDGE;
		case BHive::EWrapMode::MIRRORED_REPEAT:
			return GL_MIRRORED_REPEAT;
		case BHive::EWrapMode::CLAMP_TO_BORDER:
			return GL_CLAMP_TO_BORDER;
		default:
			break;
		}
		ASSERT(false);
		return 0;
	}

	uint32_t GetGLFormat(EFormat format)
	{
		switch (format)
		{
		case BHive::EFormat::R8:
		case BHive::EFormat::R8F:
		case BHive::EFormat::R32F:
			return GL_RED;
		case BHive::EFormat::RG8:
		case BHive::EFormat::RG16F:
		case BHive::EFormat::RG32F:
			return GL_RG;
		case BHive::EFormat::RGB8:
		case BHive::EFormat::RGB16F:
		case BHive::EFormat::RGB32F:
		case BHive::EFormat::R11_G11_B10:
			return GL_RGB;
		case BHive::EFormat::RGBA8:
		case BHive::EFormat::RGBA16F:
		case BHive::EFormat::RGBA32F:
			return GL_RGBA;
		case BHive::EFormat::RED_INTEGER:
			return GL_RED_INTEGER;
		case BHive::EFormat::RGB_UINTEGER:
			return GL_RGB_INTEGER;
		case BHive::EFormat::RGB_INTEGER:
			return GL_RGB_INTEGER;
		case BHive::EFormat::DEPTH24_STENCIL8:
			return GL_DEPTH_STENCIL;
		case BHive::EFormat::DEPTH_COMPONENT:
			return GL_DEPTH_COMPONENT;
		case BHive::EFormat::DEPTH_COMPONENT_32F:
			return GL_DEPTH_COMPONENT;
		case BHive::EFormat::DEPTH_COMPONENT_24:
			return GL_DEPTH_COMPONENT;
			break;
		default:
			break;
		}

		ASSERT(false);
		return 0;
	}

	uint32_t GetGLInternalFormat(EFormat format)
	{
		switch (format)
		{
		case BHive::EFormat::R8:
			return GL_R8;
		case BHive::EFormat::R8F:
			return GL_R8;
		case BHive::EFormat::R32F:
			return GL_R32F;
		case BHive::EFormat::RG16F:
			return GL_RG16F;
		case BHive::EFormat::RG32F:
			return GL_RG32F;
		case BHive::EFormat::RG8:
			return GL_RG8;
		case BHive::EFormat::RGB8:
			return GL_RGB8;
		case BHive::EFormat::RGBA8:
			return GL_RGBA8;
		case BHive::EFormat::RGB16F:
			return GL_RGB16F;
		case BHive::EFormat::RGBA16F:
			return GL_RGBA16F;
		case BHive::EFormat::RGBA32F:
			return GL_RGBA32F;
		case BHive::EFormat::RGB32F:
			return GL_RGB32F;
		case BHive::EFormat::RED_INTEGER:
			return GL_R32I;
		case BHive::EFormat::RGB_UINTEGER:
			return GL_RGB32UI;
		case BHive::EFormat::RGB_INTEGER:
			return GL_RGB32I;
		case BHive::EFormat::R11_G11_B10:
			return GL_R11F_G11F_B10F;
		case BHive::EFormat::DEPTH24_STENCIL8:
			return GL_DEPTH24_STENCIL8;
		case BHive::EFormat::DEPTH_COMPONENT:
			return GL_DEPTH_COMPONENT;
		case BHive::EFormat::DEPTH_COMPONENT_32F:
			return GL_DEPTH_COMPONENT32F;
		case BHive::EFormat::DEPTH_COMPONENT_24:
			return GL_DEPTH_COMPONENT24;
			break;
		default:
			break;
		}

		ASSERT(false);
		return 0;
	}

	uint32_t GetGLType(EFormat format)
	{
		switch (format)
		{
		case BHive::EFormat::R8:
			return GL_UNSIGNED_BYTE;
		case BHive::EFormat::R8F:
			return GL_FLOAT;
		case BHive::EFormat::RG8:
			return GL_UNSIGNED_BYTE;
		case BHive::EFormat::RG16F:
			return GL_FLOAT;
		case BHive::EFormat::RGB8:
			return GL_UNSIGNED_BYTE;
		case BHive::EFormat::RGB16F:
			return GL_HALF_FLOAT;
		case BHive::EFormat::RGBA8:
			return GL_UNSIGNED_BYTE;
		case BHive::EFormat::RGBA16F:
			return GL_FLOAT;
		case BHive::EFormat::RGB32F:
			return GL_FLOAT;
		case BHive::EFormat::RGBA32F:
			return GL_FLOAT;
		case BHive::EFormat::RED_INTEGER:
			return GL_INT;
		case BHive::EFormat::RGB_UINTEGER:
			return GL_UNSIGNED_INT;
		case BHive::EFormat::RGB_INTEGER:
			return GL_INT;
		case BHive::EFormat::R11_G11_B10:
			return GL_FLOAT;
		case BHive::EFormat::DEPTH24_STENCIL8:
			return GL_UNSIGNED_INT_24_8;
		case BHive::EFormat::DEPTH_COMPONENT_32F:
			return GL_FLOAT;
		case BHive::EFormat::DEPTH_COMPONENT_24:
			return GL_UNSIGNED_INT;
			break;
		default:
			break;
		}

		ASSERT(false);
		return 0;
	}

	EFormat BHive::GetFormat(uint32_t channels)
	{
		switch (channels)
		{
		case 1:
			return EFormat::R8;
		case 2:
			return EFormat::RG8;
		case 3:
			return EFormat::RGB8;
		case 4:
			return EFormat::RGBA8;
		}
		ASSERT(false);
		return (EFormat)0;
	}

	bool IsDepthFormat(EFormat format)
	{
		return format == EFormat::Depth || format == EFormat::DEPTH_COMPONENT_32F || format == EFormat::DEPTH_COMPONENT_24;
	}

	uint32_t GetDepthAttachmentType(EFormat format)
	{
		switch (format)
		{
		case BHive::EFormat::DEPTH24_STENCIL8:
			return GL_DEPTH_STENCIL_ATTACHMENT;
		case BHive::EFormat::DEPTH_COMPONENT:
		case BHive::EFormat::DEPTH_COMPONENT_32F:
		case BHive::EFormat::DEPTH_COMPONENT_24:
			return GL_DEPTH_ATTACHMENT;
		default:
			break;
		}
		ASSERT(false);
		return 0;
	}

	EFormat GetFormatFromChannels(bool hdr, int channels)
	{
		if (hdr)
		{
			switch (channels)
			{
			case 3:
				return EFormat::RGB32F;
			case 4:
				return EFormat::RGBA32F;
			default:
				break;
			}
		}
		else
		{
			switch (channels)
			{
			case 1:
				return EFormat::R8;
			case 2:
				return EFormat::RG8;
			case 3:
				return EFormat::RGB8;
			case 4:
				return EFormat::RGBA8;
			default:
				break;
			}
		}

		switch (channels)
		{
		case 1:
			return EFormat::R8;
		case 2:
			return EFormat::RG8;
		case 3:
			return EFormat::RGB8;
		case 4:
			return EFormat::RGBA8;
		default:
			break;
		}

		ASSERT(false);
		return EFormat::Invalid;
	}

	uint32_t GetGLAccess(EImageAccess access)
	{
		switch (access)
		{
		case EImageAccess::READ:
			return GL_READ_ONLY;
		case EImageAccess::WRITE:
			return GL_WRITE_ONLY;
		case EImageAccess::READ_WRITE:
			return GL_READ_WRITE;
		default:
			break;
		}
	}
} // namespace BHive
