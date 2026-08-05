#include "ComputeBindings.h"
#include "gfx/material/BackendMaterial.h"

namespace BHive
{
	FComputeBindings::FComputeBindings(const Ref<ShaderProgram> &program)
		: mProgram(program)
	{
		mBackendMaterial = IMaterialBackendInterface::Create(program);
	}

	IMaterial &FComputeBindings::SetTexture(const std::string &name, const FTextureBinding &texture) &
	{
		mBackendMaterial->SetTexture(name, texture);
		return *this;
	}

	IMaterial &FComputeBindings::SetParam(const std::string &name, const MaterialParam &value) &
	{
		mBackendMaterial->SetParam(name, value);
		return *this;
	}
} // namespace BHive