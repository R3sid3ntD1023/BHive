#include "Platform/Vulkan/VulkanVertexArray.h"
#include "RenderCommand.h"
#include "VertexArray.h"
#include "rendergraph/Pass.h"

namespace BHive
{
	void VertexArray::DeclareAccess(FPass &pass, EBufferUsage vbAccess, EBufferUsage ibAccess)
	{
		for (auto &vb : GetVertexBuffers())
			pass.UseBuffer(vb, vbAccess);

		if (auto ib = GetIndexBuffer())
			pass.UseBuffer(ib, ibAccess);
	}

} // namespace BHive