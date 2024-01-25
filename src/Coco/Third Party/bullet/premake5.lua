project "bullet"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    OutputBin = "%{OutputDir.bin}Third Party/%{prj.name}"
    OutputObj = "%{OutputDir.obj}Third Party/%{prj.name}"

    targetdir(OutputBin)
    objdir(OutputObj)

    files
    {
        "%{wks.location}/src/Vendor/bullet3/src/*.h",
        "%{wks.location}/src/Vendor/bullet3/src/BulletCollision/**.h",
        "%{wks.location}/src/Vendor/bullet3/src/BulletCollision/**.cpp",
        "%{wks.location}/src/Vendor/bullet3/src/BulletDynamics/**.h",
        "%{wks.location}/src/Vendor/bullet3/src/BulletDynamics/**.cpp",
        "%{wks.location}/src/Vendor/bullet3/src/BulletInverseDynamics/**.h",
        "%{wks.location}/src/Vendor/bullet3/src/BulletInverseDynamics/**.cpp",
        "%{wks.location}/src/Vendor/bullet3/src/BulletSoftBody/**.h",
        "%{wks.location}/src/Vendor/bullet3/src/BulletSoftBody/**.cpp",
        "%{wks.location}/src/Vendor/bullet3/src/LinearMath/**.h",
        "%{wks.location}/src/Vendor/bullet3/src/LinearMath/**.cpp",
        "**.h",
        "**.cpp"
    }

    includedirs
    {
        "%{IncludeDir.bullet}"
    }

    -- Build configs
    filter { "configurations:Debug" }
        defines {"_DEBUG=1"}

        editandcontinue "off"
        floatingpoint "fast"

        runtime "Debug"
        symbols "on"
        
    filter { "configurations:Release" }
        vectorextensions "SSE2"
        floatingpoint "fast"

        runtime "Release"
        optimize "on"