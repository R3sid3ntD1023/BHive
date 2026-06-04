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

		virtual void Init(Pipeline* pipeline) = 0;

		virtual void Bind(Pipeline* pipeline) = 0;

		virtual void Shutdown() = 0;

		virtual void BindTexture(const std::string &name, const Ref<Texture> &texture, uint32_t mip , Pipeline* pipeline) = 0;

		virtual void Set(const std::string &name, const void *data, size_t size) = 0;

		template<typename TParam>
		void Set(const std::string &name, const TParam &value)
		{
			Set(name, &value, sizeof(TParam));
		}

		virtual const FSetReflection &GetTargetSet() const = 0;

		static Scope<IMaterialBackendInterface> Create();
	};
}