#pragma once

#include "gfx/Shader.h"
#include "ShaderAsset.h"

namespace BHive
{
	class BHIVE_API ShaderProgram : public Shader
	{
	public:
		ShaderProgram(const Ref<ShaderAsset> &asset);

		virtual ~ShaderProgram() = default;

		virtual const std::string &GetName() const override { return mAsset->Name; }

		virtual const FShaderReflection &GetRefl() const override { return mAsset->MergedReflection; }

		const ShaderAsset &GetAsset() const { return *mAsset; }

		Ref<ShaderAsset> GetAssetRef() const { return mAsset; }

	private:
		Ref<ShaderAsset> mAsset;
	};
}