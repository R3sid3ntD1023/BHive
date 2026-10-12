#pragma once

#include "Commands.h"
#include "core/Core.h"
#include "Pass.h"

namespace BHive
{
	class Graph
	{
	public:
		FPass &AddPass(const std::string &name, EPassType type, FPassState state = {});

		void Append(Graph graph);

		void ResolveBufferTransitions();

		bool IsEmpty() const { return mPasses.size() == 0; };

		const auto &GetPasses() const { return mPasses; };

		auto &GetPasses() { return mPasses; };

		void DebugPrint();

	private:
		std::list<FPass> mPasses;
	};

} // namespace BHive