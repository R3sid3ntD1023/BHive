#pragma once

#include "ShaderAsset.h"
#include "gfx/NativeHandle.h"
#include "gfx/ResourceID.h"

namespace BHive
{
	class BHIVE_API Shader : public INativeObject
	{
	public:
		Shader(const ShaderAsset &asset)
			: mAsset(asset) {};

		virtual ~Shader() = default;

		virtual const std::string &GetName() const { return mAsset.Name; }

		virtual const FShaderReflection &GetMergedRefl() const { return mAsset.MergedReflection; }

		virtual const FShaderReflectionLookUp &GetRefl() const { return mAsset.LookupTable; }

		const ShaderAsset &GetAsset() const { return mAsset; }

		const ResourceID &GetResourceID() const { return mResourceID; }

	private:
		ShaderAsset mAsset;

		ResourceID mResourceID;
	};
} // namespace BHive