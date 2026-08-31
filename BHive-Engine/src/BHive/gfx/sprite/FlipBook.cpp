#include "FlipBook.h"

namespace BHive
{
	FlipBook::FlipBook(const std::vector<FlipBook::Frame> &frames)
		: mFrames(frames)
	{
	}

	void FlipBook::Play()
	{
		mIsPlaying = true;
		mCurrentTime = 0.0f;
	}

	void FlipBook::Stop()
	{
		mIsPlaying = false;
		mCurrentTime = 0.0f;
	}

	void FlipBook::Update(float deltatime)
	{
		if (mIsPlaying)
		{
			mCurrentTime += deltatime;

			auto total_time = GetTotalTime();
			if (mCurrentTime >= total_time && mIsLooping)
			{
				mCurrentTime = 0.0f;
			}
		}
	}

	void FlipBook::SetLoop(bool loop)
	{
		mIsLooping = loop;
	}

	void FlipBook::SetFramesPerSecond(float fps)
	{
		mFramesPerSecond = fps;
	}

	void FlipBook::AddFrame(SpritePtr sprite, uint32_t duration)
	{
		mFrames.emplace_back(Frame{.Sprite = sprite, .Duration = duration});
	}

	void FlipBook::InsertFrame(SpritePtr sprite, uint32_t duration, uint32_t index)
	{
		mFrames.insert(mFrames.begin() + index, Frame{.Sprite = sprite, .Duration = duration});
	}

	SpritePtr FlipBook::RemoveSprite(uint32_t index)
	{
		if (mFrames.size() > index)
		{
			return mFrames[index].Sprite;
		}

		return {};
	}

	void FlipBook::SetFrames(const std::vector<FlipBook::Frame> &frames)
	{
		mFrames = frames;
	}

	SpritePtr FlipBook::GetCurrentSprite() const
	{
		return GetSpriteAtTime(mCurrentTime);
	}

	int32_t FlipBook::GetNumFrames() const
	{
		int32_t sum = 0;
		for (int32_t i = 0; i < mFrames.size(); i++)
		{
			sum += mFrames[i].Duration;
		}

		return sum;
	}

	int32_t FlipBook::GetFrameIndexAtTime(float time) const
	{
		if (time < 0.0f)
			return -1;

		if (mFramesPerSecond > 0.0f)
		{
			float sum = 0.0f;
			for (int32_t i = 0; i < mFrames.size(); i++)
			{
				sum += mFrames[i].Duration / mFramesPerSecond;

				if (time <= sum)
					return i;
			}

			return (int32_t)(mFrames.size() - 1);
		}
		else
		{
			return (mFrames.size() > 0) ? 0 : -1;
		}
	}

	SpritePtr FlipBook::GetSpriteAtFrame(int32_t frame) const
	{
		if (frame < 0 || frame >= mFrames.size())
			return {};

		return mFrames.at(frame).Sprite;
	}

	SpritePtr FlipBook::GetSpriteAtTime(float time) const
	{
		const auto index = GetFrameIndexAtTime(time);
		return (index != -1) ? mFrames[index].Sprite : SpritePtr{};
	}

	float FlipBook::GetTotalTime() const
	{
		if (mFramesPerSecond != 0)
		{
			return GetNumFrames() / mFramesPerSecond;
		}

		return 0.0f;
	}

	void FlipBook::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(mIsLooping, mFramesPerSecond, mFrames);
	}

	void FlipBook::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		ar(mIsLooping, mFramesPerSecond, mFrames);
	}

	REFLECT(FlipBook)
	{
		{
			BEGIN_REFLECT(FlipBook::Frame) REFLECT_PROPERTY(Sprite) REFLECT_PROPERTY(Duration);
		}

		{
			BEGIN_REFLECT(FlipBook)
			REFLECT_CONSTRUCTOR()
			REFLECT_PROPERTY("Loop", IsLooping, SetLoop)
			REFLECT_PROPERTY("FramesPerSecond", GetFramesPerSecond, SetFramesPerSecond)
			REFLECT_PROPERTY("Frames", GetFrames, SetFrames);
		}

		rttr::type::register_wrapper_converter_for_base_classes<Ref<FlipBook>>();
	}
} // namespace BHive