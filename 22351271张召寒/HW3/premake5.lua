workspace "Ani_assignment3"
	architecture "x64"
	
	configurations
	{
		"Debug",
		"Release",
	}
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Ani_assignment3/vendor/GLFW/include"
IncludeDir["Glad"] = "Ani_assignment3/vendor/Glad/include"
IncludeDir["glm"] = "Ani_assignment3/vendor/glm"
IncludeDir["stb_image"] = "Ani_assignment3/vendor/stb_image"
IncludeDir["assimp"] = "Ani_assignment3/vendor/Assimp/include"

-- include other premake5 files
include "Ani_assignment3/vendor/GLFW"
include "Ani_assignment3/vendor/Glad"
-- include "Ani_assignment3/vendor/assimp"

project "Ani_assignment3"
	location "Ani_assignment3"
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
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/Assimp/include/assimp/**.hpp",
        "%{prj.name}/vendor/Assimp/include/assimp/**.h",
        "%{prj.name}/vendor/Assimp/include/assimp/**.inl"
	}
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}
	includedirs
	{
		"Ani_assignment3/src",
		"Ani_assignment3/vendor",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.assimp}"-- dll location
		-- "%{IncludeDir.assimp}/assimp" -- Location of assimp's config.h, for a template see include/assimp/config.h.in
	}
	libdirs 
	{ 
		"Ani_assignment3/vendor/Assimp/lib" 
	}
	links
	{
		"GLFW",
		"Glad",
		"opengl32.lib",
		"assimp-vc142-mtd.lib"
		-- "assimp.lib"
		-- "assimp"
	}

	-- postbuildcommands
	-- {
	-- 	("{COPYFILE} Ani_assignment3/vendor/Assimp/bin/assimp-vc142-mtd.dll bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}")
	-- }
	--默认的初始路径并非premake5.lua所在路径而是项目根路径也就是Ani_assignment3/
		postbuildcommands
	{
		("{COPYFILE} vendor/Assimp/bin/assimp-vc142-mtd.dll ../bin/" .. outputdir .. "/Ani_assignment3")
	}



	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
