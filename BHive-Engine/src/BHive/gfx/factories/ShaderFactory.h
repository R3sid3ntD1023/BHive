#pragma once

#include "gfx/factories/IResourceFactory.h"
#include "gfx/shader/Shader.h"

namespace BHive
{
	class BHIVE_API ShaderFactory : public IResourceFactory<Shader>
	{
	public:
		static ShaderPtr Create(const std::filesystem::path &path);

		static ShaderPtr Create(const std::string &name, const std::string &vert, const std::string &frag);

		static ShaderPtr Create(const ShaderAsset &asset);
	};

} // namespace BHive