#include "BufferBase.h"
#include "RenderCommand.h"

namespace BHive
{
	void BufferBase::SetData(const void *data, size_t size, uint32_t offset)
	{
		RenderGraph graph{};
		auto &pass = graph.AddPass("BufferUpload", EPassType::OffScreen);

		pass.BeginPhase(EPhaseType::Transfer);
		pass.Emplace<CmdUploadBuffer>()(this, data, size, offset);
		pass.EndPhase();

		Renderer::Get().ExecuteGraph(graph);
	}
}