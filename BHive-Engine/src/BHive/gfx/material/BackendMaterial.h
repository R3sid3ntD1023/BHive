#pragma once

#include "MaterialSnapshot.h"
#include "asset/Asset.h"
#include "core/Core.h"

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

	class IMaterialBackendInterface
	{
	public:
		virtual ~IMaterialBackendInterface() = default;

		virtual void SetTexture(const std::string &name, const TextureBinding &texture) = 0;

		virtual void SetParam(const std::string &name, const MaterialParam &value) = 0;

		virtual MaterialSnapshot CreateSnapshot() const = 0;

		static Ref<IMaterialBackendInterface> Create(const std::string &shaderProgramName);
	};

	class BHIVE_API IMaterial
	{
	public:
		virtual ~IMaterial() = default;

		virtual IMaterial &SetTexture(const std::string &name, const TextureBinding &texture) & = 0;
		virtual IMaterial &SetTexture(const std::string &name, const TextureBinding &texture) && = delete;

		virtual IMaterial &SetParam(const std::string &name, const MaterialParam &value) & = 0;
		virtual IMaterial &SetParam(const std::string &name, const MaterialParam &value) && = delete;

		virtual MaterialSnapshot CreateSnapshot() const = 0;
	};
} // namespace BHive