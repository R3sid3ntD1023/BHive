#pragma once

#include "core/Core.h"


namespace BHive
{
	class Pipeline;
	class Texture;
	struct FSetReflection;

	class IMaterialBackendInterface
	{
	public:
		virtual ~IMaterialBackendInterface() = default;

		virtual void Init(const Ref<Pipeline> &shader) = 0;

		virtual void Bind(const Ref<Pipeline> &shader) = 0;

		virtual void Shutdown() = 0;

		virtual void BindTexture(const std::string& name, const Ref<Texture> &texture) = 0;

		virtual void Set(const std::string &name, const void *data, size_t size) = 0;

		virtual const FSetReflection &GetTargetSet() const = 0;

		static Ref<IMaterialBackendInterface> Create();
	};
}