#include "FlipBook.h"

namespace BHive
{
	FlipBook::FlipBook(std::initializer_list<Frame> frames)
		: mFrames()
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

	void FlipBook::AddFrame(const Ref<Sprite> &sprite, uint32_t duration)
	{
		mFrames.emplace_back(Frame{.Sprite = sprite, .Duration = duration});
	}

	void FlipBook::InsertFrame(const Ref<Sprite> &sprite, uint32_t duration, uint32_t index)
	{
		mFrames.insert(mFrames.begin() + index, Frame{.Sprite = sprite, .Duration = duration});
	}

	Ref<Sprite> FlipBook::RemoveSprite(uint32_t index)
	{
		if (mFrames.size() > index)
		{
			return mFrames[index].Sprite;
		}

		return nullptr;
	}

	const FlipBook::Frames &FlipBook::GetFrames() const
	{
		return mFrames;
	}

	void FlipBook::SetFrames(const Frames &frames)
	{
		mFrames = frames;
	}

	Ref<Sprite> FlipBook::GetCurrentSprite() const
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

	Ref<Sprite> FlipBook::GetSpriteAtFrame(int32_t frame) const
	{
		if (frame < 0 || frame >= mFrames.size())
			return nullptr;

		return mFrames.at(frame).Sprite;
	}

	Ref<Sprite> FlipBook::GetSpriteAtTime(float time) const
	{
		const auto index = GetFrameIndexAtTime(time);
		return (index != -1) ? mFrames[index].Sprite : nullptr;
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
		ar(mIsLooping, mFramesPerSecond);

		ar(cereal::make_size_tag(mFrames.size()));
		for (auto &frame : mFrames)
		{
			ar(TAssetHandle<Sprite>(frame.Sprite), frame.Duration);
		}
	}

	void FlipBook::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		ar(mIsLooping, mFramesPerSecond);

		size_t size = 0;
		ar(cereal::make_size_tag(size));

		mFrames.resize(size);
		for (auto &frame : mFrames)
		{
			ar(TAssetHandle<Sprite>(frame.Sprite), frame.Duration);
		}
	}

	REFLECT(FlipBook)
	{
		{
			BEGIN_REFLECT(Frame) REFLECT_PROPERTY(Sprite) REFLECT_PROPERTY(Duration);
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