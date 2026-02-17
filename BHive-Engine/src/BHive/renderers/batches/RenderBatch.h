#pragma once

namespace BHive
{
	class ShaderProgram;

	struct IRenderBatch
	{
		virtual ~IRenderBatch() = default;

		virtual void End() = 0;

		virtual void StartBatch() = 0;

		virtual void NextBatch() = 0;

		virtual void Flush() = 0;
	};
} // namespace BHive