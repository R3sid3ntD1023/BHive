#pragma once

namespace BHive
{
	class Framebuffer;
	class Window;

	class RenderGraph
	{
	public:
		struct Pass
		{
			std::string Name;
			Framebuffer *Target = nullptr;
			std::function<void()> Execute;
		};

		void AddPass(const std::string &name, Framebuffer *target, std::function<void()> execute) { mPasses.emplace_back(name, target, execute); }

		const std::vector<Pass> &GetPasses() const { return mPasses; }

		void Clear() { mPasses.clear(); }

	private:
		std::vector<Pass> mPasses;
	};
}