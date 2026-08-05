#pragma once

#include "BackendMaterial.h"
#include "gfx/resources/ImageSubresourceRange.h"
#include "gfx/shader/ShaderProgram.h"

namespace BHive
{
	class Texture;
	class ShaderProgram;

	class BHIVE_API FComputeBindings : public IMaterial
	{
	public:
		FComputeBindings(const Ref<ShaderProgram> &program);

		virtual ~FComputeBindings() = default;

		IMaterial &SetTexture(const std::string &name, const FTextureBinding &texture) & override;

		IMaterial &SetParam(const std::string &name, const MaterialParam &value) & override;

		Ref<IMaterialBackendInterface> GetNative() const override { return mBackendMaterial; }

		MaterialSnapshot CreateSnapshot() const override { return mBackendMaterial->CreateSnapshot(); }

		Ref<ShaderProgram> GetProgram() const override { return mProgram; }

	private:
		Ref<IMaterialBackendInterface> mBackendMaterial;

		Ref<ShaderProgram> mProgram;
	};
} // namespace BHive