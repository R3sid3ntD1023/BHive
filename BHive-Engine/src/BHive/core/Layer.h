#pragma once

#include "core/Core.h"

namespace BHive
{
	class Event;
	class Application;

	class BHIVE_API Layer
	{
	public:
		virtual ~Layer() = default;

		virtual void OnAttach(Application&) {};
		virtual void OnDetach() {};
		virtual void OnUpdate(float) {};
		virtual void OnEvent(Event &) {};
		virtual void OnGuiRender() {}
	};

	class BHIVE_API LayerStack
	{
	public:
		using Layers = std::vector<Ref<Layer>>;

	public:
		LayerStack() = default;

		~LayerStack();

		void Push(const Ref<Layer>& layer);

		void Pop(const Ref<Layer> &layer);

		Layers::iterator begin() { return mLayers.begin(); }
		Layers::iterator end() { return mLayers.end(); }

		Layers::const_iterator begin() const { return mLayers.begin(); }
		Layers::const_iterator end() const { return mLayers.end(); }

		Layers::reverse_iterator rbegin() { return mLayers.rbegin(); }
		Layers::reverse_iterator rend() { return mLayers.rend(); }

		Layers::const_reverse_iterator rbegin() const { return mLayers.rbegin(); }
		Layers::const_reverse_iterator rend() const { return mLayers.rend(); }

	private:
		Layers mLayers;
	};
} // namespace BHive