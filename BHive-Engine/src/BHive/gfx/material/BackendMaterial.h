#pragma once

#include "core/Core.h"
#include "MaterialSnapshot.h"
#include "gfx/resources/ImageSubResourceRange.h"

namespace BHive
{
	class Pipeline;
	class Texture;
	struct FSetReflection;
	struct FShaderReflection;

	struct FTextureBinding
	{
		std::string name;
		Ref<Texture> texture;
		ImageSubresourceRange range = {};
	};

	template <typename TValue>
	struct FValueBinding
	{
		std::string name;
		TValue value;
	};


	class IMaterialBackendInterface
	{
	public:
		virtual ~IMaterialBackendInterface() = default;

		virtual void Init(Pipeline* pipeline) = 0;

		virtual void BindTexture(const std::string &name, const Ref<Texture> &texture, uint32_t mip , Pipeline* pipeline) = 0;

		virtual void Set(const std::string &name, const void *data, size_t size) = 0;

		template<typename TParam>
		void Set(const std::string &name, const TParam &value)
		{
			Set(name, &value, sizeof(TParam));
		}

		virtual const FSetReflection &GetTargetSet() const = 0;

		virtual const FShaderReflection *GetRefl() const = 0;

		virtual MaterialSnapshot CreateSnapshot() const = 0;

		static Ref<IMaterialBackendInterface> Create();
	};

	class BHIVE_API IMaterial
	{
	public:
		virtual ~IMaterial() = default;

		virtual MaterialSnapshot CreateSnapshot() const = 0;

		virtual Pipeline* GetPipeline() const = 0;

		virtual Ref<IMaterialBackendInterface> GetNative() const = 0;
	};
}