project "yaml-cpp"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    OutputBin = "%{OutputDir.bin}%{prj.name}"
    OutputObj = "%{OutputDir.obj}%{prj.name}"

    targetdir(OutputBin)
    objdir(OutputObj)

    defines { "YAML_CPP_STATIC_DEFINE" }

    files
    {
        "%{wks.location}\\src\\Vendor\\yaml-cpp\\src\\**.h",
        "%{wks.location}\\src\\Vendor\\yaml-cpp\\src\\**.cpp"
    }

    includedirs
    {
        "%{IncludeDir.yaml_cpp}",
    }

    links
    {}

    -- Build configs
    filter { "configurations:Debug" }
        runtime "Debug"
        symbols "on"
        
    filter { "configurations:Release" }
        runtime "Release"
        optimize "on"