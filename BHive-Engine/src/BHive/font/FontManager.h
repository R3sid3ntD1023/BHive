#pragma once

#include "core/Core.h"

namespace BHive
{
	class Font;

	class FontManager
	{
	public:
		FontManager();

		void AddFont(const std::string &name, const Ref<Font> &font);

		Ref<Font> AddFontFromFile(const std::filesystem::path &filename, int fontSize);

		void SetDefaultFont(const Ref<Font> &font);

		Ref<Font> GetDefaultFont() const { return mDefaultFont; }

		static FontManager &Get();

	private:
		std::unordered_map<std::string, Ref<Font>> mFonts;
		Ref<Font> mDefaultFont;
	};

} // namespace BHive
