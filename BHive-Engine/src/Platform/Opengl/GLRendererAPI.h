#pragma once

#include "gfx/RendererAPI.h"
#include "gfx/rendergraph/Graph.h"

namespace BHive
{
	struct GLTextureInfo
	{
		uint32_t Target;

		uint32_t Format;

		int32_t InternalFormat;

		uint32_t Type;

		uint32_t MipLevels = 1;

		uint32_t WrapMode;

		uint32_t MinFilter;

		uint32_t MagFilter;

		std::optional<uint32_t> CompareMode;

		std::optional<uint32_t> CompareOp;

		glm::vec4 BorderColor = {0, 0, 0, 0};
	};

	class GLRendererAPI : public RendererAPI
	{
	public:
		GLRendererAPI();
		GLRendererAPI(const GLRendererAPI &) = delete;
		GLRendererAPI &operator=(const GLRendererAPI &) = delete;

		~GLRendererAPI();

		void Init() override;

		void Shutdown() override;

		void SubmitGraph(const Graph &graph) override;

		void RenderFrame();

		void QueueDeletion(FQeueuDeletionFunc &&fn) override;

		void CreateTexture(uint32_t &id, ETextureType type, const glm::uvec3 &size, const FTextureCreateInfo &info) override;

		void DeleteTexture(uint32_t &textureID) override;

		void SetTextureData(uint32_t textureID, const void *data, size_t size, ImageCopyRegion region, ImageSubresourceRange range) override;

		void CreateBuffer(uint32_t &id, EBufferType type, EBufferLifetime lifeTime, size_t size, const char *debugName = nullptr) override;

		void DeleteBuffer(uint32_t &bufferID) override;

		void SetBufferData(uint32_t bufferID, const void *data, size_t size, uint32_t offset) override;

		void ClearBuffer(uint32_t bufferID) override;

	private:
		void ExecuteFinalGraph(Graph &graph);

		void BeginOffScreenRendering(const FPassState &state, const FPhase &phase);

		void EndOffScreenRendering(const FPassState &state, const FPhase &phase);

		void ExecuteCommands(const FPhase &phase);

	private:
		std::vector<Graph> mSubmittedGraphs;
		std::unordered_map<uint32_t, GLTextureInfo> mTextureInfos;
		uint32_t mFullscreenVertexArray = 0;
	};
} // namespace BHive