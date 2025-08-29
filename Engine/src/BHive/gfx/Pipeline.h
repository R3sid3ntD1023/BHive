#pragma once

#include "core/Core.h"
#include "ShaderStages.h"

namespace BHive
{
	class Shader;

	class Pipeline
	{
	public:
		Pipeline();

		~Pipeline();

		void Init();

		void Release();

		void Bind();

		void UnBind();

		void UseShaderStage(EShaderStage stage, const Ref<Shader> &shader);

	private:
		uint32_t mPipelineID{0};
	};
} // namespace BHive