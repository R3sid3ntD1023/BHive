#pragma once

#include "core/Core.h"
#include "RendererAPI.h"
#include "ShaderReflection.h"

namespace BHive
{
	class Texture;

	class BHIVE_API Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() = 0;

		virtual void UnBind() = 0;

		virtual const std::string &GetName() const = 0;

		virtual void Dispatch(uint32_t w, uint32_t h, uint32_t d = 1) = 0;

		template <typename T>
		void SetUniform(const std::string &name, const T &val) {};

		virtual void Save(cereal::BinaryOutputArchive &ar) const {};

		virtual void Load(cereal::BinaryInputArchive &ar) {};

		virtual const FShaderReflectionData& GetRefl() const = 0;

		static Ref<Shader> Create(const std::filesystem::path &path);

		static Ref<Shader> Create(const std::string &name, const std::string &vert, const std::string &frag);

		virtual void Reflect() = 0;

		friend class ShaderSerializer;
	};

	

} // namespace BHive

#include "Shader.inl"