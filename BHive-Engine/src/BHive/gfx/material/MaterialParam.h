#pragma once

namespace BHive
{
	struct MaterialParam
	{
		std::vector<std::byte> Data;
		size_t Size = 0;

		MaterialParam() = default;

		explicit MaterialParam(size_t size)
			: Data(size),
			  Size(size)
		{
		}

		template <typename T>
		explicit MaterialParam(const T &value)
		{
			Size = sizeof(T);
			Data.resize(Size);
			memcpy(Data.data(), &value, Size);
		}

		template <typename Ar>
		void Serialize(Ar &ar)
		{
			ar(Data, Size);
		}
	};
} // namespace BHive