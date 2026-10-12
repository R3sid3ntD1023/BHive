#pragma once

#include "core/Core.h"
#include "core/math/Math.h"

namespace BHive
{
	struct MSDFData;
	class Texture2D;

	struct GlyphBounds
	{
		glm::dvec2 Min{};
		glm::dvec2 Max{};
	};

	class Font
	{
	private:
		/* data */
	public:
		Font(const std::filesystem::path &path, int fontSize);
		~Font();

		const MSDFData *GetMSDFData() const { return mData; }

		Ref<Texture2D> GetAtlas() const { return mTextureAtlas; }

	private:
		void Initialize(const std::filesystem::path &path);

	private:
		int mFontSize = 0;
		float mAscender = 0;
		float mDescender = 0;
		int mLineHeight = 0;
		float mDeviceScale;

		Ref<Texture2D> mTextureAtlas;
		MSDFData *mData = nullptr;
	};

} // namespace BHive
