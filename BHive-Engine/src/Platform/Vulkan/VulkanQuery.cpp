#include "VulkanQuery.h"
#include "VulkanBackend.h"

namespace BHive
{
	VulkanQuery::VulkanQuery(uint32_t count)
		: mCount(count + 1)
	{
		auto &device = VulkanBackend::GetLogicalDevice();

		vk::QueryPoolCreateInfo info({}, vk::QueryType::eTimestamp, mCount);
		mQueryPool = device.createQueryPool(info);
	}

	void VulkanQuery::Reset(vk::CommandBuffer cmd)
	{
		cmd.resetQueryPool(mQueryPool, 0, mCount);
		mWasReset = true;
	}

	void VulkanQuery::Write(vk::CommandBuffer cmd, vk::PipelineStageFlags2 stage, uint32_t query)
	{
		cmd.writeTimestamp2(stage, mQueryPool, query);
	}

	std::vector<float> VulkanQuery::GetResults() const
	{
		auto period = VulkanBackend::GetPhysicalDevice().getProperties().limits.timestampPeriod;

		std::vector<float> results;
		results.reserve(mResults.size());

		for (auto &r : mResults)
		{
			results.emplace_back(float(r) * period / 1000000.0f);
		}

		return results;
	}

	void VulkanQuery::QueryResults()
	{
		if (!mWasReset)
			return;

		auto results = mQueryPool.getResults<uint64_t>(0, mCount, mCount * sizeof(uint64_t), sizeof(uint64_t), vk::QueryResultFlagBits::e64);
		mResultsReady = (results.result == vk::Result::eSuccess);
		mResults = mResultsReady ? results.value : std::vector<uint64_t>();
	}

} // namespace BHive