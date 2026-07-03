#include "ComputeBindings.h"
#include "gfx/material/BackendMaterial.h"

namespace BHive
{
	FComputeBindings::FComputeBindings(Pipeline *pipeline)
		: mPipeline(pipeline)
	{
		mBackendMaterial = IMaterialBackendInterface::Create();
		mBackendMaterial->Init(pipeline);
	}

	FComputeBindings &FComputeBindings::Set(const FTextureBinding &b)
	{
		BindTextureInternal(b.name, b.texture, b.range);
		return *this;
	}

	void FComputeBindings::SetInternal(const std::string & name, const void *data, size_t size)
	{
		mBackendMaterial->Set(name, data, size);
	}

	void FComputeBindings::BindTextureInternal(const std::string &name, Ref<Texture> tex, ImageSubresourceRange range)
	{
		mBackendMaterial->BindTexture(name, tex, range.BaseMipLevel, mPipeline);
	}
} // namespace BHive