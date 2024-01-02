workspace "Ani_assignment2"
	architecture "x64"
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Ani_assignment2/vendor/GLFW/include"
IncludeDir["Glad"] = "Ani_assignment2/vendor/Glad/include"
IncludeDir["glm"] = "Ani_assignment2/vendor/glm"
IncludeDir["stb_image"] = "Ani_assignment2/vendor/stb_image"

-- include other premake5 files
include "Ani_assignment2/vendor/GLFW"
include "Ani_assignment2/vendor/Glad"


project "Ani_assignment2"
	location "Ani_assignment2"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/VertexObject/**.h",
		"%{prj.name}/src/VertexObject/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}
	includedirs
	{
		"Ani_assignment2/src",
		"Ani_assignment2/src/VertexObject",
		"Ani_assignment2/vendor",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
	}

	links
	{
		"GLFW",
		"Glad",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		optimize "On"