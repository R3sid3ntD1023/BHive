#pragma once

#include "core/Core.h"

namespace BHive
{
	class Texture;

	class BHIVE_API IImGuiTexture
	{
	public:
		virtual ~IImGuiTexture() = default;

		virtual uint64_t GetTextureID(const Texture &tex) = 0;

		virtual void InvalidateTexture(const Texture &tex) = 0;
	};

	class BHIVE_API IImGuiTextureProvider
	{
	public:
		static void Init();

		static void Shutdown();

		static uint64_t GetID(const Texture &tex);

		static void Invalidate(const Texture &tex);

	private:
		static inline Scope<IImGuiTexture> sBackend;
	};
}