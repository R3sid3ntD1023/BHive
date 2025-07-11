#pragma once

#include "gfx/Color.h"
#include "math/Math.h"
#include "PostProcessor.h"

namespace BHive
{
	class Shader;

	class Aces : public PostProcessor
	{

	public:
		Aces(uint32_t w, uint32_t h);

		virtual Ref<Texture> Process(const Ref<Texture> &texture) override;

		virtual void Resize(uint32_t w, uint32_t h) override;

	private:
		void Initialize(uint32_t w, uint32_t h);

	private:
		Ref<Texture> mOutput;
		Ref<Shader> mComputeShader;
	};
} // namespace BHive