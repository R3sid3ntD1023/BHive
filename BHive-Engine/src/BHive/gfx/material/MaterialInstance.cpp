#include "MaterialInstance.h"
#include "core/utils/Hash.h"
#include "gfx/shader/ShaderReflection.h"

namespace BHive
{
	MaterialInstance::MaterialInstance(const Ref<Material> &parent)
		: mParent(parent)
	{
	}

	IMaterial &MaterialInstance::SetParam(const std::string &name, const MaterialParam &val) &
	{
		mUniformOverrides[name] = val;
		return *this;
	}

	IMaterial &MaterialInstance::SetTexture(const std::string &name, const FTextureBinding &texture) &
	{
		mTextureOverrides[name] = texture;
		return *this;
	}

	MaterialSnapshot MaterialInstance::CreateSnapshot() const
	{
		auto base = mParent->CreateSnapshot();
		auto refl = base.mReflection;

		for (auto &[name, slot] : mTextureOverrides)
		{
			auto hash = utils::ComputeHash(name);
			if (!base.Textures.contains(hash))
				continue;

			if (auto sampler = refl->FindSampler(name, 1))
			{
				base.Textures[hash] = {slot.Texture, sampler->Binding, slot.BaseMipLevel, slot.BaseArrayLayer};
			}
		}

		if (!mUniformOverrides.empty())
		{

			auto &pc = refl->PushConstants;

			for (auto &[hash, data] : mUniformOverrides)
			{
				if (auto member = refl->FindPushConstant(hash))
				{
					memcpy(base.PushConstantData.data() + member->Offset, data.Data.data(), data.Size);
				}
			}
		}

		return base;
	}

} // namespace BHive