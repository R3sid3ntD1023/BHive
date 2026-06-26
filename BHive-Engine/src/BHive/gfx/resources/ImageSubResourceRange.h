#pragma once

namespace BHive
{
	struct ImageSubresourceRange
	{
		uint32_t BaseMipLevel = 0;
		uint32_t LevelCount = 1;
		uint32_t BaseArrayLayer = 0;
		uint32_t LayerCount = 1;
	};

	inline std::string to_string(ImageSubresourceRange range)
	{
		std::stringstream ss;
		ss << "[" << range.BaseMipLevel << "," << range.LevelCount << "," << range.BaseArrayLayer << "," << range.LayerCount << "]";
		return ss.str();
	}
}