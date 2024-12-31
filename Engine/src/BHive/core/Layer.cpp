#include "Layer.h"

namespace BHive
{
	LayerStack::~LayerStack()
	{
		for (auto layer : mLayers)
		{
			layer->OnDetach();
			delete layer;
		}
	}
	void LayerStack::Push(Layer* layer)
	{
		mLayers.emplace_back(layer);
		layer->OnAttach();
	}

	void LayerStack::Pop(Layer* layer)
	{
		auto it = std::find(begin(), end(), layer);

		if (it != end())
		{
			layer->OnDetach();
			mLayers.erase(it);
		}
	}
}