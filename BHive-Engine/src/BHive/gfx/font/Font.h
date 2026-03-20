#pragma once

#include "core/Core.h"
#include <glm/glm.hpp>

namespace BHive
{
	class Texture2D;

	struct MSDFData;

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
		Font(const char *filename, int fontSize);
		~Font();

		const MSDFData *GetMSDFData() const { return mData; }

		const Ref<Texture2D> &GetAtlas() const { return mTextureAtlas; }

	private:
		void Initialize(const char *filename);

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
