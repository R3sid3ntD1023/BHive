#include "Pipeline.h"
#include "Shader.h"
#include <glad/glad.h>

namespace BHive
{
	Pipeline::Pipeline()
	{
	}
	Pipeline::~Pipeline()
	{
		Release();
	}

	void Pipeline::Init()
	{
		glGenProgramPipelines(1, &mPipelineID);
	}

	void Pipeline::Release()
	{
		glDeleteProgramPipelines(1, &mPipelineID);
	}

	void Pipeline::Bind()
	{
		glBindProgramPipeline(mPipelineID);
	}

	void Pipeline::UnBind()
	{
		glBindProgramPipeline(0);
	}

	void Pipeline::UseShaderStage(EShaderStage stage, const Ref<Shader> &shader)
	{
		glUseProgramStages(mPipelineID, stage, shader->GetRendererID());
	}
} // namespace BHive