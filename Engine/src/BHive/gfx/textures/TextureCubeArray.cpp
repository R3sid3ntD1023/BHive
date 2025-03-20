#include "TextureCubeArray.h"
#include "gfx/TextureUtils.h"
#include <glad/glad.h>

namespace BHive
{
	TextureCubeArray::TextureCubeArray(
		uint32_t width, uint32_t height, uint32_t depth, const FTextureSpecification &specification)
		: mWidth(width),
		  mHeight(height),
		  mDepth(depth),
		  mSpecification(specification)
	{

		glCreateTextures(GL_TEXTURE_CUBE_MAP_ARRAY, 1, &mTextureID);

		glTextureStorage3D(
			mTextureID, specification.Levels, GetGLInternalFormat(specification.InternalFormat), width, height, depth);

		if (mSpecification.Levels > 1)
		{
			glGenerateTextureMipmap(mTextureID);
		}

		glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, GetGLFilterMode(specification.MinFilter));
		glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, GetGLFilterMode(specification.MagFilter));

		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_R, GetGLWrapMode(specification.WrapMode));
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, GetGLWrapMode(specification.WrapMode));
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, GetGLWrapMode(specification.WrapMode));
		glTextureParameteri(mTextureID, GL_TEXTURE_CUBE_MAP_SEAMLESS, GL_TRUE);

		if (mSpecification.WrapMode == EWrapMode::CLAMP_TO_BORDER)
		{
			glTextureParameterfv(mTextureID, GL_TEXTURE_BORDER_COLOR, mSpecification.BorderColor);
		}

		if (IsDepthFormat(specification.InternalFormat))
		{
			if (specification.CompareMode.has_value())
			{
				glTextureParameteri(
					mTextureID, GL_TEXTURE_COMPARE_MODE, GetTextureCompareMode(specification.CompareMode.value()));
			}

			if (specification.CompareFunc.has_value())
			{
				glTextureParameteri(
					mTextureID, GL_TEXTURE_COMPARE_FUNC, GetTextureCompareFunc(specification.CompareFunc.value()));
			}
		}

		// mResourceHandle = glGetTextureHandleNV(mTextureID);

		// if (!glIsTextureHandleResidentNV(mResourceHandle))
		// 	glMakeTextureHandleResidentNV(mResourceHandle);
	}

	TextureCubeArray::~TextureCubeArray()
	{
		// if (glIsTextureHandleResidentNV(mResourceHandle))
		// 	glMakeTextureHandleNonResidentNV(mResourceHandle);
		glDeleteTextures(1, &mTextureID);
	}

	void TextureCubeArray::Bind(uint32_t slot) const
	{

		glBindTextureUnit(slot, mTextureID);
	}

	void TextureCubeArray::UnBind(uint32_t slot) const
	{

		glBindTextureUnit(slot, 0);
	}

	void TextureCubeArray::BindAsImage(uint32_t unit, EImageAccess image_access, uint32_t level) const
	{
		auto access = GetGLAccess(image_access);
		glBindImageTexture(unit, mTextureID, level, GL_FALSE, 0, access, GetGLInternalFormat(mSpecification.InternalFormat));
	}

	void TextureCubeArray::GenerateMipMaps() const
	{

		glGenerateTextureMipmap(mTextureID);
	}

	void TextureCubeArray::SetData(const void *data, uint64_t size, uint32_t offsetX, uint32_t offsetY)
	{

		glTextureSubImage3D(
			mTextureID, 0, offsetX, offsetY, 0, mWidth, mHeight, mDepth, GetGLFormat(mSpecification.InternalFormat),
			GetGLType(mSpecification.InternalFormat), data);
	}
} // namespace BHive