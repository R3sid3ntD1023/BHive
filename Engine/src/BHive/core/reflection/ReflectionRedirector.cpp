#include "core/serialization/Serialization.h"
#include "ReflectionRedirector.h"

namespace BHive
{
	ReflectionRedirector::ReflectionRedirector(const std::filesystem::path &filename)
	{
		if (!std::filesystem::exists(filename))
		{
			std::ofstream out(filename);
			cereal::JSONOutputArchive ar(out);
			ar(MAKE_NVP("RedirectedTypes", mRedirectedTypes));
			return;
		}

		std::ifstream in(filename);
		cereal::JSONInputArchive ar(in);
		ar(MAKE_NVP("RedirectedTypes", mRedirectedTypes));
	}

	const std::string ReflectionRedirector::Redirect(const std::string &value) const
	{
		if (mRedirectedTypes.contains(value))
			return mRedirectedTypes.at(value);

		return value;
	}
} // namespace BHive