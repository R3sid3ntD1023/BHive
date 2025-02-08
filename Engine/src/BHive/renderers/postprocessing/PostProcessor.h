#pragma once

#include "core/Core.h"

namespace BHive
{
	class Texture;

	class PostProcessor
	{
	public:
		virtual Ref<Texture> Process(const Ref<Texture> &texture) = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
	};
} // namespace BHive