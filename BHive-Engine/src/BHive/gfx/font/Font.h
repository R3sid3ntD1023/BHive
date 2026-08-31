#pragma once

#include "core/Core.h"
#include <glm/glm.hpp>
#include "gfx/registries/Handles.h"

namespace BHive
{
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

		TexturePtr GetAtlas() const { return mTextureAtlas; }

	private:
		void Initialize(const char *filename);

	private:
		int mFontSize = 0;
		float mAscender = 0;
		float mDescender = 0;
		int mLineHeight = 0;
		float mDeviceScale;

		TexturePtr mTextureAtlas;
		MSDFData *mData = nullptr;
	};

} // namespace BHive
