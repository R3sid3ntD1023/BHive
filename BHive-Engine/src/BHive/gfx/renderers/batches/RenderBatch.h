#pragma once


namespace BHive
{
	class Renderer;

	struct IRenderBatch
	{
		virtual ~IRenderBatch() = default;

		virtual void Initialize() = 0;

		virtual void StartBatch() = 0;

		virtual void Flush(Renderer& renderer) = 0;

		virtual bool NeedsFlush(uint32_t vNeeded, uint32_t iNeeded) = 0;

		void NextBatch(Renderer &renderer)
		{
			Flush(renderer);
			StartBatch();
		}
	};
} // namespace BHive