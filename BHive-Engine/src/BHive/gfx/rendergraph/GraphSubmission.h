#pragma once

#include "Graph.h"

namespace BHive
{
	class GraphSubmission
	{
	public:
		void Submit(Graph graph);

		Graph Take();

		void Clear();

		bool IsEmpty() const { return mGraph.IsEmpty(); }

	private:
		Graph mGraph;
	};
} // namespace BHive
