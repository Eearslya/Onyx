include "./vendor/premake/solution_items.lua"

workspace "Onyx"
	architecture "x86_64"
	startproject "Onyx.Sandbox"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

	solution_items {
		".gitignore",
		".clang-format"
	}

	flags {
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Onyx.Engine/vendor/GLFW/include"
IncludeDir["spdlog"] = "%{wks.location}/Onyx.Engine/vendor/spdlog/include"

group "Utilities"
	include "vendor/premake"
group "Dependencies"
	include "Onyx.Engine/vendor/GLFW"
group ""

include "Onyx.Engine"
include "Onyx.Sandbox"