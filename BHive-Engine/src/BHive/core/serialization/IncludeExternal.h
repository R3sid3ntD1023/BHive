#pragma once

#include "Serialization.h"
#include <filesystem>
#include <type_traits>

namespace BHive
{
	template<typename T>
	struct IncludeExternal
	{
		using PT = typename std::conditional<
			std::is_const<typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value, const void *,
			void *>::type;

		IncludeExternal(T &&obj, const std::filesystem::path &path)
			: mObject(std::forward<T>(obj)),
			  mPath(path)
		{}

		template<typename A>
		void Save(A& ar) const
		{
			//save object to path
			{
				std::ofstream out(mPath, std::ios::binary | std::ios::out);
				if (!out)
					return "";

				A sub_ar(out);
				sub_ar(mObject);
			}

			ar(MAKE_NVP("External:" , mPath));
		}

		template<typename A>
		void Load(A& ar)
		{

			ar(MAKE_NVP("External:", mPath));
			
			//load object from path
			{
				std::ifstream in(mPath, std::ios::binary | std::ios::in);
				if (!in)
					return "";

				A sub_ar(in);
				sub_ar(mObject);
			}
		}

	private:
		PT mObject;
		std::filesystem::path mPath;
	};
}
