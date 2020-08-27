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

group "Utilities"
	include "vendor/premake"
group ""

include "Onyx.Engine"
include "Onyx.Sandbox"