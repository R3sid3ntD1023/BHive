#pragma once

#include "gfx/Query.h"
#include "VulkanCore.h"

namespace BHive
{
	class BHIVE_API VulkanQuery : public Query
	{
	public:
		VulkanQuery(uint32_t count);

		void Reset(vk::CommandBuffer cmd);

		void Write(vk::CommandBuffer cmd, vk::PipelineStageFlags2 stage, uint32_t query);

		void QueryResults();

		bool IsResultsReady() const override { return mResultsReady; }

		std::vector<float> GetResults() const override;

	private:
		uint32_t mCount;
		vk::raii::QueryPool mQueryPool = VK_NULL_HANDLE;
		std::vector<uint64_t> mResults;
		bool mResultsReady = false;
		bool mWasReset = false;
	};
} // namespace BHive