#include "gfx/Shader.h"
#include "ShaderSerializer.h"
#include "ShaderUtils.h"

namespace BHive
{
	void ShaderSerializer::Serialize(const std::filesystem::path &filename, const Shader &shader)
	{
		auto name = filename.stem().string();
		auto cached_path = ShaderUtils::GetCacheDirectory() / name / (name + ".cached.program");

		if (!std::filesystem::exists(cached_path.parent_path()))
			std::filesystem::create_directory(cached_path.parent_path());

		std::ofstream out(cached_path, std::ios::out | std::ios::binary);
		if (!out)
			return;

		cereal::BinaryOutputArchive ar(out);

		shader.Save(ar);

		LOG_INFO("Wrote Program Binary to: {}", cached_path);
	}

	bool ShaderSerializer::Deserialize(const std::filesystem::path &filename, Shader &shader)
	{
		auto name = filename.stem().string();
		auto cached_path = ShaderUtils::GetCacheDirectory() / name / (name + ".cached.program");

		std::ifstream in(cached_path, std::ios::in | std::ios::binary);
		if (!in)
			return false;

		cereal::BinaryInputArchive ar(in);

		shader.Load(ar);

		LOG_INFO("Read Program Binary from: {}", cached_path);

		return true;
	}
} // namespace BHive