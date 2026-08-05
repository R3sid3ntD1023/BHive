#pragma once

#include "PostProcessMaterial.h"

namespace BHive
{
	class PostProcessStack
	{
	public:
		template <typename T>
			requires(std::is_base_of_v<PostProcessMaterial, T>)
		T *Add(const std::string &name)
		{
			auto mat = CreateRef<T>();
			auto raw = mat.get();
			Materials.emplace(name, mat);
			return raw;
		}

		template <typename T>
			requires(std::is_base_of_v<PostProcessMaterial, T>)
		T *Get(const std::string &name) const
		{
			return Materials.contains(name) ? dynamic_cast<T *>(Materials.at(name).get()) : nullptr;
		}

		void Resize(const glm::uvec2 &size, PostProcessAllocator &allocator)
		{
			allocator.Resize(size);

			for (auto &[_, mat] : Materials)
			{
				if (!mat)
					continue;

				mat->OnResize(size, allocator);
			}
		}

		Ref<Texture> Build(RenderGraph &graph, PostProcessAllocator &allocator, Ref<Texture> input)
		{
			for (auto &[_, mat] : Materials)
			{
				if (!mat)
					continue;

				input = mat->AddToGraph(graph, allocator, input);
			}

			return input;
		}

	private:
		std::unordered_map<std::string, Ref<PostProcessMaterial>> Materials;
	};
} // namespace BHive