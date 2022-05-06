workspace "VulkanApp"
    architecture "x64"
    startproject "VulkanApp"

    configurations {
        "Debug",
        "Release"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["Vulkan"] = "VulkanApp/vendor/Vulkan/include"
IncludeDir["GLFW"] = "VulkanApp/vendor/GLFW/Include"
IncludeDir["spdlog"] = "VulkanApp/vendor/spdlog/Include"
IncludeDir["VMA"] = "VulkanApp/vendor/VulkanMemoryAllocator/include"

include "VulkanApp/vendor/GLFW"

project "VulkanApp"
    location "VulkanApp"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    flags {
        "MultiProcessorCompile"
    }

    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs {
        "%{prj.name}/src",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Vulkan}",
        "%{IncludeDir.VMA}"
    }
    
    libdirs {
        "%{prj.name}/vendor/Vulkan/Lib"
    }

    links {
        "GLFW",
        "vulkan-1.lib"
    }


    defines {
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLFW_INCLUDE_VULKAN",
            "VKAPP_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "VKAPP_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "VKAPP_RELEASE"
		optimize "On"