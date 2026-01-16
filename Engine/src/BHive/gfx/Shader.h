#pragma once

#include "core/Core.h"
#include "ShaderReflection.h"

namespace BHive
{
	class Texture;

	class BHIVE_API Shader
	{
	public:
		enum EShaderStage
		{
			ShaderStage_None = 0,
			ShaderStage_Vertex,
			ShaderStage_Fragment,
			ShaderStage_Compute,
			ShaderStage_Geometry,
		};

	public:
		virtual ~Shader() = default;

		virtual void Bind() = 0;

		virtual void UnBind() = 0;

		virtual const std::string &GetName() const = 0;

		virtual void Dispatch(uint32_t w, uint32_t h, uint32_t d = 1) = 0;

		template <typename T>
		void SetUniform(const std::string &name, const T &val) {};

		virtual void BindTexture(uint32_t binding, const Ref<Texture> &texture) = 0;

		virtual const FShaderReflectionData &GetRelectionData() const = 0;

		virtual void Save(cereal::BinaryOutputArchive &ar) const {};

		virtual void Load(cereal::BinaryInputArchive &ar) {};

		static Ref<Shader> Create(const std::filesystem::path &path);

		static Ref<Shader> Create(const std::string &name, const std::string &vert, const std::string &frag);

		friend class ShaderSerializer;
	};

} // namespace BHive

#include "Shader.inl"