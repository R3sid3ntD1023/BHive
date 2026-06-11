#include "GlobalResources.h"

namespace BHive
{
	void GlobalResources::Register(const std::string &semantic, const Ref<BufferBase> &buffer)
	{
		GlobalResource r;
		r.ResourceKind = GlobalResource::Kind::Buffer;
		r.BufferRef = buffer;
		mResources[semantic] = r;
	}

	void GlobalResources::Register(const std::string &semantic, const Ref<Texture> &texture)
	{
		GlobalResource r;
		r.ResourceKind = GlobalResource::Kind::Texture;
		r.TextureRef = texture;
		mResources[semantic] = r;
	}

	const GlobalResources::GlobalResource *GlobalResources::Find(const std::string &semantic) const
	{
		auto it = mResources.find(semantic);
		return it != mResources.end() ? &it->second : nullptr;
	}

	std::string GlobalResources::DebugListSemantics() const
	{
		std::string out = "{";
		for (auto &[name, res] : mResources)
		{
			if (res.IsBuffer())
				out += name + "(buffer), ";
			if (res.IsTexture())
				out += name + "(texture), ";
		}
		out += "}";
		return out;
	}

	std::string GlobalResources::GuessSemanticFromName(const std::string &semantic) const
	{
		for (auto &[name, _] : mResources)
			if (semantic.find(name) != std::string::npos)
				return name;
		return {};
	}
} // namespace BHive