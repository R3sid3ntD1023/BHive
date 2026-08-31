#include "MaterialInstance.h"
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
			if (base.Textures.contains(name))
			{
				if (auto sampler = refl->FindSampler(name, 1))
					base.Textures[name] = {slot.Texture, sampler->Binding, slot.BaseMipLevel, slot.BaseArrayLayer};
			}
		}

		if (!mUniformOverrides.empty())
		{

			auto &pc = refl->PushConstants;

			for (auto &[name, data] : mUniformOverrides)
			{
				for (auto &p : pc)
				{
					if (p.Members.contains(name))
					{
						auto &u = p.Members.at(name);
						memcpy(base.PushConstantData.data() + u.Offset, data.Data.data(), data.Size);
					}
				}
			}
		}

		return base;
	}

} // namespace BHive