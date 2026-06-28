#pragma once

#include "core/Core.h"

namespace BHive
{
	class Texture;
	class Material;
	class Pipeline;
	class Framebuffer;
	class PQuad;
	class Renderer;

	struct FDebugTextureEntry
	{
		std::string Name;
		Ref<Texture> Tex;
		bool IsCube = false;
		uint32_t MipLevels = 1;
		uint32_t Layers = 1;
	};

	class BHIVE_API ImageDebugger
	{
	public:

		static ImageDebugger& Get()
		{
			static ImageDebugger sInstance;
			return sInstance;
		}

		void Initialize(const glm::uvec2 &size);

		void RegisterTexture(const std::string &name, const Ref<Texture> &tex);

		void OnRender(Renderer &renderer);

		void OnGuiRender();

	private:

		ImageDebugger() = default;

		std::unordered_map<std::string, size_t> mTextures;
		std::vector<FDebugTextureEntry> mTextureEntries;

		int32_t mSelected = -1;
		int32_t mSelectedMip = 0;
		int32_t mSelectedFace = 0;

		Ref<Framebuffer> mFB;
		Ref<Pipeline> mPipeline;
		Ref<Material> mMaterial;
		glm::uvec2 mSize{};
	};
}