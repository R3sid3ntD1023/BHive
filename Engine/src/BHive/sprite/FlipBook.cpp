#include "FlipBook.h"
#include "asset/AssetManager.h"

namespace BHive
{
	FlipBook::FlipBook(std::initializer_list<Frame> frames)
		:mFrames()
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

	void FlipBook::AddSprite(const sprite_ptr& sprite, uint32_t duration, uint32_t index)
	{
		mFrames.insert(mFrames.begin() + index, Frame{ .mSprite = sprite, .mDuration = duration });
	}

	sprite_ptr FlipBook::RemoveSprite(uint32_t index)
	{
		if (mFrames.size() > index)
		{
			return mFrames[index].mSprite;
		}

		return nullptr;
	}

	const FlipBook::Frames& FlipBook::GetFrames() const
	{
		return mFrames;
	}

	void FlipBook::SetFrames(const Frames& frames)
	{
		mFrames = frames;
	}

	sprite_ptr FlipBook::GetCurrentSprite() const
	{
		return GetSpriteAtTime(mCurrentTime);
	}

	void FlipBook::Serialize(StreamWriter& ar) const
	{
		Asset::Serialize(ar);
		ar(mIsLooping, mFramesPerSecond, mFrames);
	}

	void FlipBook::Deserialize(StreamReader& ar)
	{
		Asset::Deserialize(ar);
		ar(mIsLooping, mFramesPerSecond, mFrames);
	}

	int32_t FlipBook::GetNumFrames() const
	{
		int32_t sum = 0;
		for (int32_t i = 0; i < mFrames.size(); i++)
		{
			sum += mFrames[i].mDuration;
		}

		return sum;
	}

	int32_t FlipBook::GetFrameIndexAtTime(float time) const
	{
		if (time < 0.0f) return -1;

		if (mFramesPerSecond > 0.0f)
		{
			float sum = 0.0f;
			for (int32_t i = 0; i < mFrames.size(); i++)
			{
				sum += mFrames[i].mDuration / mFramesPerSecond;

				if (time <= sum) return i;
			}

			return (int32_t)(mFrames.size() - 1);
		}
		else
		{
			return (mFrames.size() > 0) ? 0 : -1;
		}
	}

	sprite_ptr FlipBook::GetSpriteAtFrame(int32_t frame) const
	{
		if (frame < 0) return nullptr;

		int32_t sum = 0;
		for (int32_t i = 0; i < mFrames.size(); i++)
		{
			if (frame == sum) return mFrames[i].mSprite;

			sum += mFrames[i].mDuration;
		}

		return nullptr;
	}

	sprite_ptr FlipBook::GetSpriteAtTime(float time) const
	{
		const auto index = GetFrameIndexAtTime(time);
		return (index != -1) ? mFrames[index].mSprite : nullptr;
	}

	float FlipBook::GetTotalTime() const
	{
		if (mFramesPerSecond != 0)
		{
			return GetNumFrames() / mFramesPerSecond;
		}

		return 0.0f;
	}

	void Serialize(StreamWriter& ar, const FlipBook::Frame& obj)
	{
		ar(obj.mDuration, obj.mSprite);
	}

	void Deserialize(StreamReader& ar, FlipBook::Frame& obj)
	{
		ar(obj.mDuration, obj.mSprite);
	}

	REFLECT(FlipBook::Frame)
	{
		BEGIN_REFLECT(FlipBook::Frame)
			REFLECT_CONSTRUCTOR()
			CONSTRUCTOR_POLICY_OBJECT
			REFLECT_PROPERTY("Duration", mDuration)
			REFLECT_PROPERTY("Source", mSprite);
	}

	REFLECT(FlipBook)
	{
		BEGIN_REFLECT(FlipBook)
		REFLECT_CONSTRUCTOR()
			REFLECT_PROPERTY("Loop", IsLooping, SetLoop)
			REFLECT_PROPERTY("FramesPerSecond", GetFramesPerSecond, SetFramesPerSecond)
			REFLECT_PROPERTY("Frames", GetFrames, SetFrames);
	}
	
}