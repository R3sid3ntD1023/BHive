#pragma once

#include "core/Core.h"

namespace BHive
{
	class Pipeline;
	class Texture;

	class IMaterialBackendInterface
	{
	public:
		virtual ~IMaterialBackendInterface() = default;

		virtual void Init(const Ref<Pipeline> &shader) = 0;

		virtual void Bind(const Ref<Pipeline> &shader) = 0;

		virtual void Shutdown() = 0;

		virtual void BindTexture(uint32_t slot, const Ref<Texture> &texture) = 0;

		static Ref<IMaterialBackendInterface> Create();
	};
}