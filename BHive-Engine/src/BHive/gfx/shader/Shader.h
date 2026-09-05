#pragma once

#include "ShaderAsset.h"
#include "ShaderTemplate.h"
#include "gfx/NativeHandle.h"
#include "gfx/ResourceID.h"

namespace BHive
{
	class BHIVE_API Shader : public INativeObject
	{
	public:
		Shader(const ShaderAsset &asset)
			: mAsset(asset)
		{
			mTemplate = ShaderTemplate::Build(mAsset.MergedReflection);
		};

		virtual ~Shader() = default;

		virtual const std::string &GetName() const { return mAsset.Name; }

		const ShaderAsset &GetAsset() const { return mAsset; }

		const ResourceID &GetResourceID() const { return mResourceID; }

		const ShaderTemplate &GetTemplate() const { return mTemplate; }

	private:
		ShaderTemplate mTemplate;

		ShaderAsset mAsset;

		ResourceID mResourceID;
	};
} // namespace BHive