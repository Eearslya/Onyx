project "Onyx.Engine"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	
	targetdir ("%{wks.location}/bin/" .. outputdir)
	objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "src/pch.cpp"

	files {
		"src/**.h",
		"src/**.cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl"
	}

	defines {
		"ONYX_BUILD_DLL"
	}

	includedirs {
		"src",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.spdlog}"
	}

	links {
		"Glad",
		"GLFW",
		"imgui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"
		defines "GLFW_INCLUDE_NONE"

	filter "configurations:Debug"
		defines "ONYX_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ONYX_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ONYX_DIST"
		runtime "Release"
		optimize "on"