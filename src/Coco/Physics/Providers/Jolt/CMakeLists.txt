target_sources(Physics PRIVATE
)

FetchContent_Declare(
        jolt
        GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
        GIT_TAG v5.5.0
)

FetchContent_MakeAvailable(jolt)

target_include_directories(Physics PRIVATE
        ${jolt_SOURCE_DIR}/Jolt
)