#pragma once

#include "core/Core.h"

namespace BHive
{
	class BHIVE_API RenderDocAPI
	{
	public:
		RenderDocAPI() = default;

		void Init();

		void StartCaptureWithFile(const std::filesystem::path &path = "");

		void StartCapture();

		void EndCapture();
	};
} // namespace BHive