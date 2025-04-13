#include "Platform/VulkanShader.h"
#include "TestLayer.h"

namespace BHive
{
	const char *vert = R"(
		#version 460 core

		layout(location = 0) in vec3 aPosition;
		layout(location = 1) in vec4 aColor;

		layout(location = 0) out vec4 vColor;

		void main()
		{
			gl_Position = vec4(aPosition, 1);
			vColor = aColor;
		}
)";
	const char *frag = R"(
		#version 460 core

		layout(location =0) in vec4 vColor;
		layout(location = 0) out vec4 fColor;

		void main()
		{
			fColor = vColor;
		}
	)";

	void TestLayer::OnAttach()
	{
		auto shader = CreateRef<VulkanShader>("TestShader", vert, frag);
	}
	void TestLayer::OnUpdate(float)
	{
	}
} // namespace BHive