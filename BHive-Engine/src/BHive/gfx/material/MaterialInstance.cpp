#include "MaterialInstance.h"
#include "gfx/shader/ShaderReflection.h"

namespace BHive
{
	MaterialInstance::MaterialInstance(const Ref<Material> &parent)
		: mParent(parent)
	{
	}

	void MaterialInstance::Set(const std::string &name, const Ref<Texture> &tex, uint32_t mip)
	{
		mTextureOverrides[name] = {tex, mip};
	}

	MaterialSnapshot MaterialInstance::CreateSnapshot() const
	{
		mParent->Submit();
		auto base = mParent->CreateSnapshot();

		for (auto& [name, slot] : mTextureOverrides)
		{
			if (base.Textures.contains(name))
				base.Textures.at(name).Texture = slot.Texture;
		}

		if (!mUniformOverrides.empty())
		{
			auto refl = mParent->GetNative()->GetRefl();
			auto& pc = refl->PushConstants;

			for (auto& [name, data] : mUniformOverrides)
			{
				for (auto& p : pc)
				{
					if (p.Members.contains(name))
					{
						auto &u = p.Members.at(name);
						memcpy(base.PushConstantData.data() + u.Offset, data.data(), data.size());
					}
				}

			}
		}

		return base;
	}

	void MaterialInstance::Set(const std::string &name, const void *data, size_t size)
	{
		std::vector<std::byte> buf(size);
		memcpy(buf.data(), data, size);
		mUniformOverrides[name] = std::move(buf);
	}
} // namespace BHive