#pragma once

#include "BackendMaterial.h"
#include "gfx/resources/ImageSubresourceRange.h"

namespace BHive
{
	class Texture;
	class Pipeline;

	class BHIVE_API FComputeBindings : public IMaterial
	{
	public:
		FComputeBindings(Pipeline *pipeline);

		virtual ~FComputeBindings() = default;

		FComputeBindings &Set(const FTextureBinding &b);

		template <typename T>
		FComputeBindings &Set(const FValueBinding<T> &b)
		{
			SetInternal(b.name, &b.value, sizeof(T));
			return *this;
		}

		Ref<IMaterialBackendInterface> GetNative() const override { return mBackendMaterial; }

		MaterialSnapshot CreateSnapshot() const override { return mBackendMaterial->CreateSnapshot(); }

		Pipeline *GetPipeline() const override { return mPipeline; }

	private:
		void SetInternal(const std::string &name, const void *data, size_t size);

		void BindTextureInternal(const std::string &name, Ref<Texture> tex, ImageSubresourceRange range);

	private:
		Ref<IMaterialBackendInterface> mBackendMaterial;
		Pipeline *mPipeline = nullptr;
	};
}