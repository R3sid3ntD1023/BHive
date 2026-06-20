#pragma once

#include "PostProcessMaterial.h"

namespace BHive
{
	class PostProcessStack
	{
	public:
		std::vector<Ref<PostProcessMaterial>> Materials;

		Ref<Texture> Build(RenderGraph &graph, PostProcessAllocator& allocator, Ref<Texture> input)
		{
			for (auto& mat : Materials)
			{
				if (!mat)
					continue;

				input = mat->AddToGraph(graph, allocator, input);
			}

			return input;
		}
	};
}