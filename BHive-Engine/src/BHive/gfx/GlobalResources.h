#pragma once

#include "core/Core.h"

namespace BHive
{
	class BufferBase;
	class Texture;

	struct BHIVE_API GlobalResources
	{
		struct GlobalResource
		{
			enum class Kind
			{
				None,
				Buffer,
				Texture
			};

			Kind ResourceKind = Kind::None;
			Ref<BufferBase> BufferRef;
			Ref<Texture> TextureRef;

			bool IsBuffer() const { return ResourceKind == Kind::Buffer; }
			bool IsTexture() const { return ResourceKind == Kind::Texture; }
		};

		void Register(const std::string &semantic, const Ref<BufferBase> &buffer);

		void Register(const std::string &semantic, const Ref<Texture> &texture);

		const GlobalResource *Find(const std::string &semantic) const;

		std::string DebugListSemantics() const;

		std::string GuessSemanticFromName(const std::string &semantic) const;

	private:
		std::unordered_map<std::string, GlobalResource> mResources;
	};

}