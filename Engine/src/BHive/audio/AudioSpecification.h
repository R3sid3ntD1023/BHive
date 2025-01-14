#pragma once

#include <optional>

namespace BHive
{
	struct FAudioSpecification
	{
		int mFormat;
		int mNumSamples;
		int mSampleRate;
		std::optional<int> mStartLoop;
		std::optional<int> mEndLoop;
	};

}