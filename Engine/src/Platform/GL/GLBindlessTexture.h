#pragma once

#include "gfx/BindlessTexture.h"

namespace BHive
{

	class GLBindlessTexture : public BindlessTexture
	{
	public:
		GLBindlessTexture(const Ref<Texture> &texture);

		virtual ~GLBindlessTexture();

		virtual uint64_t GetHandle() const override { return mResourceHandle; }

	private:
		uint64_t mResourceHandle = 0;
		Ref<Texture> mTexture;
	};

	class GLBindlessImage : public BindlessImage
	{
	public:
		GLBindlessImage(const Ref<Texture> &texture, uint32_t level, uint32_t access);

		virtual ~GLBindlessImage();

		virtual uint64_t GetHandle() const override { return mResourceHandle; }

	private:
		uint64_t mResourceHandle = 0;
	};
} // namespace BHive