#include "VertexArray.h"
#include "Platform/Vulkan/VulkanVertexArray.h"
#include "RenderCommand.h"
#include "rendergraph/Pass.h"

namespace BHive
{
	void VertexArray::DeclareAccess(FPass &pass)
	{
		for (auto &vb : GetVertexBuffers())
			pass.UseBuffer(vb, EBufferUsage::VertexRead);

		if (auto ib = GetIndexBuffer())
			pass.UseBuffer(ib, EBufferUsage::IndexRead);
	}

} // namespace BHive