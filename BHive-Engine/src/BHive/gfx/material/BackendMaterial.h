#pragma once

#include "core/Core.h"
#include "MaterialSnapshot.h"
#include "gfx/resources/ImageSubResourceRange.h"
#include "MaterialParam.h"
#include "asset/Asset.h"

namespace BHive
{
	class ShaderProgram;
	class Texture;
	struct FSetReflection;
	struct FShaderReflection;

	struct FTextureBinding
	{
		Ref<Texture> TextureRef;

		uint32_t BaseMipLevel = 0;

		uint32_t BaseArrayLayer = 0;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(TAssetHandle(TextureRef), BaseMipLevel, BaseArrayLayer);
		};
	};

	class IMaterialBackendInterface
	{
	public:
		virtual ~IMaterialBackendInterface() = default;

		virtual void SetTexture(const std::string &name, const FTextureBinding &texture) = 0;

		virtual void SetParam(const std::string &name, const MaterialParam &value) = 0;

		virtual MaterialSnapshot CreateSnapshot() const = 0;

		static Ref<IMaterialBackendInterface> Create(const Ref<ShaderProgram> &program);
	};

	class BHIVE_API IMaterial
	{
	public:
		virtual ~IMaterial() = default;

		virtual IMaterial &SetTexture(const std::string &name, const FTextureBinding &texture) & = 0;
		virtual IMaterial &SetTexture(const std::string &name, const FTextureBinding &texture) && = delete;

		virtual IMaterial &SetParam(const std::string &name, const MaterialParam &value) & = 0;
		virtual IMaterial &SetParam(const std::string &name, const MaterialParam &value) && = delete;

		virtual MaterialSnapshot CreateSnapshot() const = 0;

		virtual Ref<ShaderProgram> GetProgram() const = 0;

		virtual Ref<IMaterialBackendInterface> GetNative() const = 0;
	};
} // namespace BHive