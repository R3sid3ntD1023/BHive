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

		virtual const FShaderReflection &GetMergedRefl() const override { return mAsset->MergedReflection; }

		virtual const FShaderReflectionLookUp &GetRefl() const override { return mAsset->LookupTable; }

		const ShaderAsset &GetAsset() const { return *mAsset; }

		Ref<ShaderAsset> GetAssetRef() const { return mAsset; }

	private:
		Ref<ShaderAsset> mAsset;
	};
}