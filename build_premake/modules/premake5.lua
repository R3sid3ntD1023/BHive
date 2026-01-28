--include directories
IncludeDir = {}
Library = {}

include "vulkan"
include "glfw"
include "cereal"
include "glm"
include "spdlog"
include "entt"
include "imgui"

IncludeDir["vendor"] = vendordir
IncludeDir["glfw"] = "%{IncludeDir.vendor}/glfw/include"
IncludeDir["cereal"] = "%{IncludeDir.vendor}/cereal/include"
IncludeDir["glm"] = "%{IncludeDir.vendor}/glm"
IncludeDir["spdlog"] = "%{IncludeDir.vendor}/spdlog/include"
IncludeDir["entt"] = "%{IncludeDir.vendor}/entt/src"
IncludeDir["imgui"] = "%{IncludeDir.vendor}/imgui"