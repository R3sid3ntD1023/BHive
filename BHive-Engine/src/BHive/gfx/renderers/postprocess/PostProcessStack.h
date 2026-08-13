#pragma once

#include "PostProcessMaterial.h"

namespace BHive
{
	class PostProcessStack
	{
	public:
		void Add(Ref<PostProcessMaterial> mat)
		{
			if (!mat)
			{
				LOG_WARN("PostProcessStack: material is null. returning");
				return;
			}

			const auto &key = typeid(*mat);
			if (mUniqueMaterialIDs.contains(key))
			{
				LOG_WARN("PostProcessStack: already contains material of type {}", key.name());
				return;
			}

			mOrderedMaterials.emplace_back(mat);
			mUniqueMaterialIDs.insert(key);
			mat->Init(mSize);
		}

		template <typename T>
			requires(std::is_base_of_v<PostProcessMaterial, T>)
		T *Emplace()
		{
			auto mat = CreateRef<T>();
			Add(mat);
			return mat.get();
		}

		template <typename T>
			requires(std::is_base_of_v<PostProcessMaterial, T>)
		T *Get() const
		{
			auto key = typeid(T).hash_code();
			auto it = std::find_if(mOrderedMaterials.begin(), mOrderedMaterials.end(), [key](const auto &mat) { return key == typeid(*mat).hash_code(); });

			return it != mOrderedMaterials.end() ? std::dynamic_pointer_cast<T>(*it).get() : nullptr;
		}

		void Init(const glm::uvec2 &size)
		{
			if (size.x <= 0 || size.y <= 0)
				return;

			mSize = size;

			for (auto &mat : mOrderedMaterials)
			{
				mat->Init(size);
			}
		}

		Ref<Texture> Build(RenderGraph &graph, FPostProcessTextureSet &set)
		{
			set.PrevOutput = set.SceneColor;

			for (auto &mat : mOrderedMaterials)
			{
				set.PrevOutput = mat->AddToGraph(graph, set);
			}

			return set.PrevOutput;
		}

	private:
		std::vector<Ref<PostProcessMaterial>> mOrderedMaterials;
		std::set<std::type_index> mUniqueMaterialIDs;
		glm::uvec2 mSize{800, 600};
	};
} // namespace BHive