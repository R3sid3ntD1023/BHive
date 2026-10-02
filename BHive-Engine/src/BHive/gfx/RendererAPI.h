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

		virtual void CreateTexture2D(int32_t &id, uint32_t x, uint32_t y, const FTextureCreateInfo &info) = 0;

		virtual void CreateTexture2DArray(int32_t &id, uint32_t x, uint32_t y, const FTextureCreateInfo &info) = 0;

		virtual void CreateTextureCube(int32_t &id, uint32_t s, const FTextureCreateInfo &info) = 0;

		virtual void CreateTextureCubeArray(int32_t &id, uint32_t s, const FTextureCreateInfo &info) = 0;

		virtual void CreateTexture3D(int32_t &id, uint32_t x, uint32_t y, uint32_t z, const FTextureCreateInfo &info) = 0;

		virtual void DeleteTexture(int32_t &textureID) = 0;

		virtual void SetTextureData(int32_t textureID, const void *data, size_t size, ImageCopyRegion region, ImageSubresourceRange range) = 0;

		virtual void CreateBuffer(int32_t &id, EBufferType type, EBufferLifetime lifeTime, size_t size, const char *debugName = nullptr) = 0;

		virtual void DeleteBuffer(int32_t &bufferID) = 0;

		virtual void SetBufferData(int32_t bufferID, const void *data, size_t size, uint32_t offset) = 0;

		virtual void ClearBuffer(int32_t bufferID) = 0;

		static Scope<RendererAPI> Create();
	};
} // namespace BHive