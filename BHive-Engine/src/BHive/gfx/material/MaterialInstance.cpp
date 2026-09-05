#include "MaterialInstance.h"
#include "core/utils/Hash.h"
#include "gfx/shader/Shader.h"

namespace BHive
{
	MaterialInstance::MaterialInstance(const Ref<Material> &parent)
		: mParent(parent)
	{
	}

	IMaterial &MaterialInstance::SetParam(const std::string &name, const MaterialParam &val) &
	{
		auto hash = utils::ComputeHash(name);
		mUniformOverrides[hash] = val;
		return *this;
	}

	IMaterial &MaterialInstance::SetTexture(const std::string &name, const TextureBinding &texture) &
	{
		auto hash = utils::ComputeHash(name);
		mTextureOverrides[hash] = texture;
		return *this;
	}

	MaterialSnapshot MaterialInstance::CreateSnapshot() const
	{
		auto base = mParent->CreateSnapshot();
		auto &shaderTemplate = base.Shader.As<Shader>()->GetTemplate();

		for (auto &[nameHash, slot] : mTextureOverrides)
		{
			if (!base.Textures.contains(nameHash))
				continue;

			if (auto b = shaderTemplate.FindBinding(nameHash))
			{
				base.Textures[b->Binding] = {slot.Texture, slot.BaseMipLevel, slot.BaseArrayLayer};
			}
		}

		if (!mUniformOverrides.empty())
		{

			for (auto &[nameHash, data] : mUniformOverrides)
			{
				if (auto member = shaderTemplate.FindPushConstant(nameHash))
				{
					memcpy(base.PushConstantData.data() + member->Offset, data.Data.data(), data.Size);
				}
			}
		}

		return base;
	}

} // namespace BHive