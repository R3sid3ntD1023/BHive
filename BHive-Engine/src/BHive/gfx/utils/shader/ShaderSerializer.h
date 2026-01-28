#pragma once

#include "core/Core.h"

namespace BHive
{
	class Shader;

	struct ShaderSerializer
	{
		void Serialize(const std::filesystem::path &filename, const Shader &shader);

		bool Deserialize(const std::filesystem::path &filename, Shader &shader);
	};

} // namespace BHive