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

		template <typename A>
		void Serialize(A &ar)
		{
			ar(mFormat, mNumSamples, mSampleRate, mStartLoop, mEndLoop);
		}
	};

} // namespace BHive