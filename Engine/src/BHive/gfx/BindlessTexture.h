#pragma once

#include "core/Core.h"

namespace BHive
{
	class Texture;

	class BindlessTexture
	{
	public:
		virtual ~BindlessTexture() = default;

		virtual uint64_t GetHandle() const = 0;

		operator uint64_t() const { return GetHandle(); }

		static Ref<BindlessTexture> Create(const Ref<Texture> &texture);
	};

	class BindlessImage
	{
	public:
		virtual ~BindlessImage() = default;

		virtual uint64_t GetHandle() const = 0;

		operator uint64_t() const { return GetHandle(); }

		static Ref<BindlessImage> Create(const Ref<Texture> &texture, uint32_t level, uint32_t access);
	};
} // namespace BHive