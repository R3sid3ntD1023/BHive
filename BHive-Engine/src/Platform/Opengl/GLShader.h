#pragma once

#include "gfx/shader/Shader.h"

namespace BHive
{
	class GLResourceSet;

	class GLShader : public Shader
	{
	public:
		GLShader(const ShaderAsset &asset);
		~GLShader() override;

		void Bind();

		void BindSet(GLResourceSet *set);

		void SetPushConstants(const std::vector<std::byte> &data);

	private:
		struct PushConstantBuffer
		{
			uint32_t Buffer = 0;
			uint32_t Binding = 0;
			size_t Offset = 0;
			size_t Size = 0;
			size_t BufferSize = 0;
		};

		void CreatePushConstantBuffers();

		uint32_t mProgramID = 0;
		std::vector<PushConstantBuffer> mPushConstantBuffers;
	};
} // namespace BHive