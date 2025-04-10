#pragma once

namespace BHive
{
	class ReflectionRedirector
	{
	public:
		ReflectionRedirector(const std::filesystem::path &filename);

		const std::string Redirect(const std::string &value) const;

	private:
		std::unordered_map<std::string, std::string> mRedirectedTypes;
	};
} // namespace BHive