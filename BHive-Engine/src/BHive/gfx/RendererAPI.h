#pragma once

#include "Enumerations.h"
#include "core/Core.h"
#include "gfx/Enumerations.h"
#include "gfx/TextureSpecification.h"
#include "gfx/rendergraph/Graph.h"
#include "gfx/resources/ImageCopyRegion.h"
#include "gfx/resources/ImageSubresourceRange.h"

namespace BHive
{
	using FQeueuDeletionFunc = std::function<void(uint32_t)>;

	class BHIVE_API RendererAPI
	{
	public:
		enum EAPI
		{
			Opengl,
			Vulkan
		};

	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;

		virtual void Shutdown() = 0;

		virtual void SubmitGraph(const Graph &graph) = 0;

		virtual void QueueDeletion(FQeueuDeletionFunc &&fn) = 0;

		virtual void CreateTexture(uint32_t &id, ETextureType type, const glm::uvec3 &size, const FTextureCreateInfo &info) = 0;

		virtual void DeleteTexture(uint32_t &textureID) = 0;

		virtual void SetTextureData(uint32_t textureID, const void *data, size_t size, ImageCopyRegion region, ImageSubresourceRange range) = 0;

		virtual void CreateBuffer(uint32_t &id, EBufferType type, EBufferLifetime lifeTime, size_t size, const char *debugName = nullptr) = 0;

		virtual void DeleteBuffer(uint32_t &bufferID) = 0;

		virtual void SetBufferData(uint32_t bufferID, const void *data, size_t size, uint32_t offset) = 0;

		virtual void ClearBuffer(uint32_t bufferID) = 0;

		static Scope<RendererAPI> Create();
	};
} // namespace BHive