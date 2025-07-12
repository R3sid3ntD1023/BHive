#include "gfx/TextureUtils.h"
#include "Texture2D.h"
#include <glad/glad.h>

namespace BHive
{

	Texture2D::Texture2D(uint32_t w, uint32_t h, const FTextureSpecification &specification, const void *buffer, size_t size)
		: mWidth(w),
		  mHeight(h),
		  mSpecification(specification)
	{

		Initialize();

		if (buffer)
		{
			mBuffer.Allocate(buffer, size);
			SetData(buffer);
		}
	}

	Texture2D::~Texture2D()
	{
		Release();
		mBuffer.Release();
	}

	void Texture2D::Bind(uint32_t slot) const
	{

		glBindTextureUnit(slot, mTextureID);
	}

	void Texture2D::UnBind(uint32_t slot) const
	{
		glBindTextureUnit(slot, 0);
	}

	void Texture2D::BindAsImage(uint32_t unit, EImageAccess image_access, uint32_t level) const
	{
		auto format = GetGLInternalFormat(mSpecification.InternalFormat);
		auto access = GetGLAccess(image_access);
		glBindImageTexture(unit, mTextureID, level, GL_FALSE, 0, access, format);
	}

	void Texture2D::GenerateMipMaps() const
	{

		glGenerateTextureMipmap(mTextureID);
	}

	void Texture2D::SetSpecification(const FTextureSpecification &specs)
	{
		mSpecification.MinFilter = specs.MinFilter;
		mSpecification.MagFilter = specs.MagFilter;
		mSpecification.WrapMode = specs.WrapMode;

		glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, GetGLFilterMode(mSpecification.MinFilter));
		glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, GetGLFilterMode(mSpecification.MagFilter));
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, GetGLWrapMode(mSpecification.WrapMode));
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, GetGLWrapMode(mSpecification.WrapMode));
	}

	void Texture2D::SetData(const void *data, uint32_t offsetX, uint32_t offsetY)
	{
		glTextureSubImage2D(
			mTextureID, 0, offsetX, offsetY, mWidth, mHeight, GetGLFormat(mSpecification.InternalFormat),
			GetGLType(mSpecification.InternalFormat), data);
	}

	Ref<Texture2D> Texture2D::CreateSubTexture(const FSubTexture &texture)
	{
		auto c = mSpecification.Channels;
		size_t size = texture.width * texture.height * c;

		Buffer pixels(size);
		GetSubImage(texture, size, &pixels[0]);

		return CreateRef<Texture2D>(texture.width, texture.height, mSpecification, pixels);
	}

	void Texture2D::GetSubImage(const FSubTexture &texture, size_t size, uint8_t *data) const
	{
		auto format = GetGLFormat(mSpecification.InternalFormat);
		auto type = GetGLType(mSpecification.InternalFormat);

		glGetTextureSubImage(
			mTextureID, 0, texture.x, texture.y, texture.z, texture.width, texture.height, texture.depth, format, type, size,
			data);
	}

	void Texture2D::Initialize()
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &mTextureID);

		glTextureStorage2D(
			mTextureID, mSpecification.Levels, GetGLInternalFormat(mSpecification.InternalFormat), mWidth, mHeight);

		if (mSpecification.Levels > 1)
		{
			glGenerateTextureMipmap(mTextureID);
		}

		glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, GetGLFilterMode(mSpecification.MinFilter));
		glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, GetGLFilterMode(mSpecification.MagFilter));
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, GetGLWrapMode(mSpecification.WrapMode));
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, GetGLWrapMode(mSpecification.WrapMode));

		glGenerateTextureMipmap(mTextureID);
	}

	void Texture2D::Release()
	{
		glDeleteTextures(1, &mTextureID);

		mBuffer.Release();
	}

	void Texture2D::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(mWidth, mHeight, mSpecification, mBuffer);
	}

	void Texture2D::Load(cereal::BinaryInputArchive &ar)
	{

		Asset::Load(ar);

		ar(mWidth, mHeight, mSpecification, mBuffer);

		if (mBuffer)
		{
			Initialize();
			SetData(mBuffer);
		}
	}

	REFLECT(Texture2D)
	{
		BEGIN_REFLECT(Texture2D) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY("Specification", GetSpecification, SetSpecification);
		rttr::type::register_wrapper_converter_for_base_classes<Ref<Texture2D>>();
	}
} // namespace BHive