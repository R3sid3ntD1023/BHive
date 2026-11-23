#include "GLShader.h"
#include "GLResourceSet.h"
#include <glad/glad.h>

namespace BHive
{
	namespace utils
	{
		GLenum GetShaderType(EShaderStage s)
		{
			switch (s)
			{
			case EShaderStage::Vertex:
				return GL_VERTEX_SHADER;
			case EShaderStage::Fragment:
				return GL_FRAGMENT_SHADER;
			case EShaderStage::Compute:
				return GL_COMPUTE_SHADER;
			case EShaderStage::Geometry:
				return GL_GEOMETRY_SHADER;
			default:
				break;
			}
		}

	} // namespace utils

	GLShader::GLShader(const ShaderAsset &asset)
		: Shader(asset)
	{
		mProgramID = glCreateProgram();

		GLint status = 0;
		GLchar msg[512];
		std::vector<uint32_t> shaders;

		for (const auto &[stage, data] : asset.Stages)
		{
			auto shader = glCreateShader(utils::GetShaderType(stage));
			const auto &code = data.Spirv;

			glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, code.data(), code.size() * sizeof(uint32_t));
			glSpecializeShader(shader, "main", 0, nullptr, nullptr);
			glAttachShader(mProgramID, shader);

			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
			if (!status)
			{
				glGetShaderInfoLog(shader, 512, nullptr, msg);
				LOG_ERROR("SHADER:COMPILE ERROR {} - {}, {}", asset.Name, to_string(stage), msg);
			}

			shaders.emplace_back(shader);
		}

		glLinkProgram(mProgramID);

		glGetProgramiv(mProgramID, GL_LINK_STATUS, &status);
		if (!status)
		{
			glGetProgramInfoLog(mProgramID, 512, nullptr, msg);
			LOG_ERROR("SHADER:LINK ERROR {}, {}", asset.Name, msg);
		}

		for (auto shader : shaders)
		{
			glDetachShader(mProgramID, shader);
			glDeleteShader(shader);
		}

		if (!status)
		{
			glDeleteProgram(mProgramID);
			mProgramID = 0;
			return;
		}

		CreatePushConstantBuffers();
	}

	GLShader::~GLShader()
	{
		for (const auto &pushConstant : mPushConstantBuffers)
		{
			if (pushConstant.Buffer)
				glDeleteBuffers(1, &pushConstant.Buffer);
		}

		if (mProgramID)
			glDeleteProgram(mProgramID);
	}

	void GLShader::Bind()
	{
		glUseProgram(mProgramID);
	}

	void GLShader::BindSet(GLResourceSet *set)
	{
		if (set && GetTemplate().HasSet(set->GetSetIndex()))
			set->Update();
	}

	void GLShader::CreatePushConstantBuffers()
	{
		const auto &ranges = GetAsset().MergedReflection.PushConstants;
		if (ranges.empty())
			return;

		GLint maxBindings = 0;
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxBindings);
		if (maxBindings <= 0)
		{
			LOG_ERROR("OpenGL reports no uniform-buffer binding points for shader '{}'", GetName());
			return;
		}

		std::unordered_set<std::string> seenBlocks;
		uint32_t bindingIndex = 0;
		for (const auto &range : ranges)
		{
			if (range.Name.empty() || !seenBlocks.insert(range.Name).second)
				continue;

			GLuint blockIndex = glGetUniformBlockIndex(mProgramID, range.Name.c_str());
			if (blockIndex == GL_INVALID_INDEX)
			{
				GLint activeBlockCount = 0;
				glGetProgramiv(mProgramID, GL_ACTIVE_UNIFORM_BLOCKS, &activeBlockCount);
				std::string activeBlocks;
				for (GLint index = 0; index < activeBlockCount; ++index)
				{
					GLint nameLength = 0;
					glGetActiveUniformBlockiv(mProgramID, static_cast<GLuint>(index), GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLength);
					if (nameLength <= 1)
						continue;

					std::vector<GLchar> name(static_cast<size_t>(nameLength));
					GLsizei written = 0;
					glGetActiveUniformBlockName(mProgramID, static_cast<GLuint>(index), nameLength, &written, name.data());
					const std::string activeBlockName(name.data(), static_cast<size_t>(written));
					if (!activeBlocks.empty())
						activeBlocks += ", ";
					activeBlocks += activeBlockName;

					const std::string qualifiedName = range.Name + ".";
					if (activeBlockName.compare(0, qualifiedName.size(), qualifiedName) == 0)
					{
						blockIndex = glGetUniformBlockIndex(mProgramID, activeBlockName.c_str());
						if (blockIndex != GL_INVALID_INDEX)
							break;
					}
				}

				if (blockIndex == GL_INVALID_INDEX)
				{
					LOG_WARN("OpenGL shader '{}' has no uniform block for push constants '{}'; active blocks: [{}]", GetName(), range.Name, activeBlocks);
					continue;
				}
			}

			if (bindingIndex >= static_cast<uint32_t>(maxBindings))
			{
				LOG_ERROR("OpenGL shader '{}' exceeds the available uniform-buffer bindings", GetName());
				break;
			}

			GLint blockSize = 0;
			glGetActiveUniformBlockiv(mProgramID, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
			if (blockSize <= 0)
			{
				LOG_ERROR("OpenGL push-constant block '{}' has an incompatible size", range.Name);
				continue;
			}
			const size_t bufferSize = std::max(range.Size, static_cast<size_t>(blockSize));

			const GLuint binding = static_cast<GLuint>(maxBindings - 1) - bindingIndex++;
			glUniformBlockBinding(mProgramID, blockIndex, binding);

			GLuint buffer = 0;
			glCreateBuffers(1, &buffer);
			std::vector<std::byte> initialData(bufferSize, std::byte{0});
			glNamedBufferData(buffer, static_cast<GLsizeiptr>(bufferSize), initialData.data(), GL_DYNAMIC_DRAW);

			mPushConstantBuffers.push_back({buffer, binding, static_cast<size_t>(range.Offset), range.Size, bufferSize});
		}
	}

	void GLShader::SetPushConstants(const std::vector<std::byte> &data)
	{
		for (const auto &pushConstant : mPushConstantBuffers)
		{
			if (pushConstant.Offset > data.size() || pushConstant.Size > data.size() - pushConstant.Offset)
			{
				LOG_ERROR("OpenGL push-constant data is too small for shader '{}'", GetName());
				continue;
			}

			std::vector<std::byte> uploadData(pushConstant.BufferSize, std::byte{0});
			std::memcpy(uploadData.data(), data.data() + pushConstant.Offset, pushConstant.Size);
			glNamedBufferSubData(pushConstant.Buffer, 0, static_cast<GLsizeiptr>(pushConstant.BufferSize), uploadData.data());
			glBindBufferBase(GL_UNIFORM_BUFFER, pushConstant.Binding, pushConstant.Buffer);
		}
	}
} // namespace BHive
