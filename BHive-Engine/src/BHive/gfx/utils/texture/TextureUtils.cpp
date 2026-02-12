#include "Platform/Vulkan/VulkanBackend.h"
#include "TextureUtils.h"
#include <glad/glad.h>

namespace BHive
{

	uint32_t TextureUtils::GetAPITextureCompareMode(ETextureCompareMode mode)
	{
		switch (mode)
		{
		case BHive::ETextureCompareMode::NONE:
			return VK_FALSE;
		case BHive::ETextureCompareMode::COMPARE_REF_TO_TEXTURE:
			return VK_TRUE;
		default:
			break;
		}
		ASSERT(false);
		return 0;
	}

	uint32_t TextureUtils::GetAPITextureCompareFunc(ETextureCompareFunc func)
	{
		switch (func)
		{
		case BHive::ETextureCompareFunc::LEQUAL:
			return VK_COMPARE_OP_LESS_OR_EQUAL;
		case BHive::ETextureCompareFunc::GEQUAL:
			return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case BHive::ETextureCompareFunc::LESS:
			return VK_COMPARE_OP_LESS;
		case BHive::ETextureCompareFunc::GREATER:
			return VK_COMPARE_OP_GREATER;
		case BHive::ETextureCompareFunc::EQUAL:
			return VK_COMPARE_OP_EQUAL;
		case BHive::ETextureCompareFunc::NOTEQUAL:
			return VK_COMPARE_OP_NOT_EQUAL;
		case BHive::ETextureCompareFunc::ALWAYS:
			return VK_COMPARE_OP_ALWAYS;
		case BHive::ETextureCompareFunc::NEVER:
			return VK_COMPARE_OP_NEVER;
		default:
			break;
		}
		ASSERT(false);
		return 0;
	}

	uint32_t TextureUtils::GetTextureFormat(ETextureFormat format)
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

	uint32_t TextureUtils::GetAPIFilterMode(EMinFilter mode)
	{
		switch (mode)
		{
		case BHive::EMinFilter::LINEAR:
			return VK_FILTER_LINEAR;
		case BHive::EMinFilter::NEAREST:
			return VK_FILTER_NEAREST;
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

	uint32_t TextureUtils::GetAPIFilterMode(EMagFilter mode)
	{
		switch (mode)
		{
		case BHive::EMagFilter::LINEAR:
			return VK_FILTER_LINEAR;
		case BHive::EMagFilter::NEAREST:
			return VK_FILTER_NEAREST;
		default:
			break;
		}

		ASSERT(false);
		return 0;
	}

	uint32_t TextureUtils::GetAPIWrapMode(EWrapMode mode)
	{
		switch (mode)
		{
		case BHive::EWrapMode::REPEAT:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case BHive::EWrapMode::CLAMP_TO_EDGE:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case BHive::EWrapMode::MIRRORED_REPEAT:
			return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case BHive::EWrapMode::CLAMP_TO_BORDER:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		default:
			break;
		}
		ASSERT(false);
		return 0;
	}

	uint32_t TextureUtils::GetAPIFormat(EFormat format)
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
		case BHive::EFormat::RGBA_INTEGER:
			return GL_RGBA_INTEGER;
		case BHive::EFormat::DEPTH24_STENCIL8:
			return GL_DEPTH_STENCIL;
		case BHive::EFormat::DEPTH_COMPONENT:
		case BHive::EFormat::DEPTH_COMPONENT_32F:
		case BHive::EFormat::DEPTH_COMPONENT_24:
			return GL_DEPTH_COMPONENT;
			break;
		default:
			break;
		}

		ASSERT(false);
		return 0;
	}

	uint32_t TextureUtils::GetAPIInternalFormat(EFormat format)
	{
		switch (format)
		{
		case BHive::EFormat::R8:
			return VK_FORMAT_R8_UNORM;
		case BHive::EFormat::R8F:
			return VK_FORMAT_R8_SRGB;
		case BHive::EFormat::R32F:
			return VK_FORMAT_R32_SFLOAT;
		case BHive::EFormat::RG8:
			return VK_FORMAT_R8G8_UNORM;
		case BHive::EFormat::RG16F:
			return VK_FORMAT_R16G16_SFLOAT;
		case BHive::EFormat::RG32F:
			return VK_FORMAT_R32G32_SFLOAT;
		case BHive::EFormat::RGB8:
			return VK_FORMAT_R8G8B8_UNORM;
		case BHive::EFormat::RGBA8:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case BHive::EFormat::RGB16F:
			return VK_FORMAT_R16G16B16_SFLOAT;
		case BHive::EFormat::RGBA16F:
			return VK_FORMAT_R16G16B16A16_SFLOAT;
		case BHive::EFormat::RGB32F:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case BHive::EFormat::RGBA32F:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case BHive::EFormat::DEPTH_COMPONENT_24:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		case BHive::EFormat::DEPTH_COMPONENT_32F:
			return VK_FORMAT_D32_SFLOAT;
		case BHive::EFormat::DEPTH24_STENCIL8:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		default:
			break;
		}
		ASSERT(false);
		return VK_FORMAT_UNDEFINED;
	}

	uint32_t TextureUtils::GetAPIType(EFormat format)
	{
		switch (format)
		{
		case BHive::EFormat::R8:
		case BHive::EFormat::RG8:
		case BHive::EFormat::RGB8:
		case BHive::EFormat::RGBA8:
			return GL_UNSIGNED_BYTE;
		case BHive::EFormat::RGB16F:
			return GL_HALF_FLOAT;
		case BHive::EFormat::R8F:
		case BHive::EFormat::RG16F:
		case BHive::EFormat::RGBA16F:
		case BHive::EFormat::RGB32F:
		case BHive::EFormat::RGBA32F:
		case BHive::EFormat::R11_G11_B10:
		case BHive::EFormat::DEPTH_COMPONENT_32F:
			return GL_FLOAT;
		case BHive::EFormat::RED_INTEGER:
		case BHive::EFormat::RGB_INTEGER:
		case BHive::EFormat::RGBA_INTEGER:
			return GL_INT;
		case BHive::EFormat::RGB_UINTEGER:
		case BHive::EFormat::DEPTH_COMPONENT_24:
			return GL_UNSIGNED_INT;
		case BHive::EFormat::DEPTH24_STENCIL8:
			return GL_UNSIGNED_INT_24_8;
			break;
		default:
			break;
		}

		ASSERT(false);
		return 0;
	}

	bool TextureUtils::IsDepthFormat(EFormat format)
	{
		return format == EFormat::Depth || format == EFormat::DEPTH_COMPONENT_32F || format == EFormat::DEPTH_COMPONENT_24;
	}

	uint32_t TextureUtils::GetAPIDepthAttachmentType(EFormat format)
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

	uint32_t TextureUtils::GetAPIImageAccess(EImageAccess access)
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
