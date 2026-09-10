#include "AssimpParser.h"

namespace BHive
{
	bool AssmipParser::ProgressHandler::Update(float percentage)
	{

		bool finished = percentage == 1.f;
		auto str = std::format("\rLoading... {:.2f}%", percentage * 100.0f);
		std::cout << str << (!finished ? "" : "\n");
		return finished;
	}
} // namespace BHive