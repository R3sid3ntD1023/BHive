#pragma once

#include "PostProcessMaterial.h"

namespace BHive
{
	class PostProcessStack
	{
	public:
		std::vector<Ref<PostProcessMaterial>> Materials;

		Ref<Texture> Build(RenderGraph &graph, Ref<Texture> input)
		{
			for (auto& mat : Materials)
			{
				if (!mat)
					continue;

				input = mat->AddToGraph(graph, input);
			}

			return input;
		}

		void Resize(const glm::uvec2 &size)
		{
			for (auto &mat : Materials)
				mat->CreateResizableObjects(size);
		}
	};
}