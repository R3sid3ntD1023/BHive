#include "gfx/RenderCommand.h"
#include "Platform/Vulkan/debug/VulkanRenderDocAPI.h"
#include "RenderDoc.h"
#include <windows.h>

namespace BHive
{

	namespace details
	{
		static std::filesystem::path GenerateTiemStampedCapturePath(const std::filesystem::path &folder = {})
		{
			using namespace std::chrono;
			auto now = system_clock::now();
			std::time_t t = system_clock::to_time_t(now);
			std::tm tm;
			localtime_s(&tm, &t);

			std::ostringstream ss;
			ss << std::put_time(&tm, "%Y%m%d_%H%M%S");

			DWORD pid = GetCurrentProcessId();

			std::filesystem::path dir = folder.empty() ? (std::filesystem::current_path() / "captures") : folder;
			if (!std::filesystem::exists(dir))
			{
				std::error_code ec;
				std::filesystem::create_directories(dir, ec);
			}

			std::string filename = std::string("capture_") + ss.str() + "_pid" + std::to_string(pid) + ".rdc";
			return dir / filename;
		}
	} // namespace details

	/*void RenderDocAPI::Init()
	{
		if (rdoc_api)
			return;

		HMODULE mod = ::GetModuleHandle("renderdoc.dll");
		if (!mod)
		{
			auto path = "C:/Program Files/RenderDoc/renderdoc.dll";
			mod = ::LoadLibrary(path);
		}

		if (!mod)
			return;

		auto get_api = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
		if (!get_api)
			return;

		int ret = get_api(eRENDERDOC_API_Version_1_6_0, (void **)&rdoc_api);

		if (ret != 1)
		{
			rdoc_api = nullptr;
			return;
		}

		if (rdoc_api)
		{
			RENDERDOC_InputButton buttons[] = {RENDERDOC_InputButton::eRENDERDOC_Key_F2};
			rdoc_api->SetCaptureTitle("RenderDoc Capture");
			rdoc_api->SetCaptureKeys(buttons, 1);
			rdoc_api->ShowReplayUI();
		}
	}

	void RenderDocAPI::StartCaptureWithFile(const std::filesystem::path &path)
	{
		if (!rdoc_api)
			Init();

		if (!rdoc_api)
			return;

		auto output_path = details::GenerateTiemStampedCapturePath(path);
		rdoc_api->SetLogFilePathTemplate(output_path.string().c_str());
		rdoc_api->StartFrameCapture(GetDevice(), GetWindowHandle());
	}

	void RenderDocAPI::StartCapture()
	{
		if (!rdoc_api)
			Init();

		if (rdoc_api)
			rdoc_api->StartFrameCapture(GetDevice(), GetWindowHandle());
	}

	void RenderDocAPI::EndCapture()
	{
		if (rdoc_api)
			rdoc_api->EndFrameCapture(GetDevice(), GetWindowHandle());
	}

	void RenderDocAPI::TriggerCapture()
	{
		if (rdoc_api)
			rdoc_api->TriggerCapture();
	}*/

	Ref<APIDebugger> APIDebugger::Create()
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateRef<VulkanAPIDebugger>();
		default:
			break;
		}

		ASSERT(false)
		return nullptr;
	}

} // namespace BHive