project "Onyx.Engine"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"

	pchheader "pch.h"
	pchsource "Onyx.Engine/src/pch.cpp"
	
	targetdir ("%{wks.location}/bin/" .. outputdir)
	objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

	files {
		"src/**.h",
		"src/**.cpp"
	}

	defines {
		"ONYX_BUILD_DLL"
	}

	includedirs {
		"src",
		"%{IncludeDir.spdlog}"
	}

	filter "system:windows"
		systemversion "latest"

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