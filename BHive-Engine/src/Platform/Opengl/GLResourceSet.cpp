#include "GLResourceSet.h"
#include "gfx/BufferBase.h"
#include "gfx/Texture.h"
#include "gfx/shader/OpenGLBindingMapping.h"
#include <glad/glad.h>

namespace BHive
{
	namespace
	{
		std::optional<GLenum> ToImageFormat(EFormat format)
		{
			switch (format)
			{
			case EFormat::R8:
				return GL_R8;
			case EFormat::RG8:
				return GL_RG8;
			case EFormat::RGB8:
				return GL_RGB8;
			case EFormat::RGBA8:
				return GL_RGBA8;
			case EFormat::R16F:
				return GL_R16F;
			case EFormat::RG16F:
				return GL_RG16F;
			case EFormat::RGB16F:
				return GL_RGB16F;
			case EFormat::RGBA16F:
				return GL_RGBA16F;
			case EFormat::R32F:
				return GL_R32F;
			case EFormat::RG32F:
				return GL_RG32F;
			case EFormat::RGB32F:
				return GL_RGB32F;
			case EFormat::RGBA32F:
				return GL_RGBA32F;
			case EFormat::RED_INTEGER:
				return GL_R32I;
			case EFormat::RGB_INTEGER:
				return GL_RGB32I;
			case EFormat::RGB_UINTEGER:
				return GL_RGB32UI;
			case EFormat::RGBA_INTEGER:
				return GL_RGBA32I;
			case EFormat::R11_G11_B10:
				return GL_R11F_G11F_B10F;
			default:
				return std::nullopt;
			}
		}

		bool IsLayeredImage(const Texture *texture)
		{
			return dynamic_cast<const Texture2DArray *>(texture) ||
				dynamic_cast<const Texture3D *>(texture) ||
				dynamic_cast<const TextureCube *>(texture) ||
				dynamic_cast<const TextureCubeArray *>(texture);
		}
	} // namespace

	GLResourceSet::GLResourceSet(const BindingSetTemplate &setTemplate)
		: mSetIndex(setTemplate.SetIndex)
	{
		mBindings.reserve(setTemplate.Bindings.size());
		for (const auto &binding : setTemplate.Bindings)
			mBindings.emplace(binding.Binding, BindingInfo{binding.Type});
	}

	void GLResourceSet::SetBuffer(uint32_t binding, BufferPtr buffer)
	{
		auto it = mBindings.find(binding);
		if (it != mBindings.end() && IsBuffer(it->second.Type))
			it->second.Buffer = buffer;
	}

	void GLResourceSet::SetTexture(uint32_t binding, Ref<Texture> texture, uint32_t mip)
	{
		auto it = mBindings.find(binding);
		if (it != mBindings.end() && !IsBuffer(it->second.Type))
		{
			it->second.Texture = std::move(texture);
			it->second.MipLevel = mip;
		}
	}

	void GLResourceSet::Update()
	{
		for (const auto &[binding, info] : mBindings)
		{
			const GLuint glBinding = GetOpenGLBindingLocation(mSetIndex, binding, info.Type);
			if (IsBuffer(info.Type))
			{
				const GLenum target = info.Type == EResourceType::UniformBuffer ? GL_UNIFORM_BUFFER : GL_SHADER_STORAGE_BUFFER;
				const GLuint buffer = info.Buffer ? info.Buffer.As<BufferBase>()->GetBufferID() : 0;
				glBindBufferBase(target, glBinding, buffer);
				continue;
			}

			if (info.Type == EResourceType::StorageImage)
			{
				const GLuint texture = info.Texture ? info.Texture->GetTextureID() : 0;
				const auto format = info.Texture ? ToImageFormat(info.Texture->GetInfo().Format) : std::optional<GLenum>(GL_RGBA8);
				if (!format)
				{
					LOG_ERROR("OpenGL storage image binding {} has an unsupported format", binding);
					continue;
				}

				glBindImageTexture(
					glBinding,
					texture,
					info.MipLevel,
					info.Texture && IsLayeredImage(info.Texture.get()),
					0,
					GL_READ_WRITE,
					*format);
				continue;
			}

			if (IsTexture(info.Type))
			{
				const GLuint texture = info.Texture ? info.Texture->GetTextureID() : 0;
				glBindTextureUnit(glBinding, texture);
			}
		}
	}
}