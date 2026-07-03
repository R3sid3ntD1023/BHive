#pragma once

#include "core/Core.h"
#include "rendergraph/Pass.h"
#include "rendergraph/Commands.h"

namespace BHive
{
	class Texture;
	class Pipeline;
	class BaseMesh;
	class Material;

	struct FAsyncPass
	{
	public:

		virtual bool IsDone() = 0;

		virtual bool Wait() = 0;

		virtual void Destroy() = 0;

		virtual ~FAsyncPass() = default;
	};

	class RenderGraph
	{
	public:
	
		FPass &AddPass(const std::string &name, EPassType type);

		void Append(const RenderGraph &graph);

		bool Empty() const;

		const std::vector<FPass> &GetPasses() const;

		std::vector<FPass> &GetPasses();

		void DebugPrint();

	private:
		std::vector<FPass> mPasses;
	};

}