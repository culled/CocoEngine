option(COCO_RENDERING_INCLUDE_OPENGL "Add OpenGL rendering support (currently doesn't work with Slang)")
option(COCO_RENDERING_INCLUDE_VULKAN "Add Vulkan rendering support (requires Vulkan SDK to be installed)" ON)

add_library(Rendering STATIC
        RenderService.h
        RenderService.cpp
        Graphics/GraphicsPlatform.h
        Graphics/GraphicsPlatform.cpp
        Graphics/Resources/GraphicsSurface.cpp
        Graphics/Resources/GraphicsSurface.h
        Graphics/Resources/RenderContext.h
        Graphics/RenderFrame.cpp
        Graphics/RenderFrame.h
        RenderScene.cpp
        RenderScene.h
        RenderGraph/RenderGraphBuilder.cpp
        RenderGraph/RenderGraphBuilder.h
        RenderGraph/RenderGraph.cpp
        RenderGraph/RenderGraph.h
        RenderGraph/RenderGraphTypes.h
        RenderGraph/RenderGraphTypes.cpp
        Texture.cpp
        Texture.h
        Graphics/Resources/Image.cpp
        Graphics/Resources/Image.h
        Graphics/Resources/ImageTypes.h
        RenderGraph/RenderGraphStorage.cpp
        RenderGraph/RenderGraphStorage.h
        Graphics/Resources/ImageTypes.cpp
        Graphics/GraphicsResource.cpp
        Graphics/GraphicsResource.h
        Graphics/Resources/RenderContext.cpp
        Mesh.cpp
        Mesh.h
        Shader.cpp
        Shader.h
        Graphics/VertexDataTypes.h
        Graphics/Resources/ShaderProgram.cpp
        Graphics/Resources/ShaderProgram.h
        ShaderTypes.h
        Graphics/Resources/BufferTypes.h
        Graphics/Resources/BufferTypes.cpp
        Graphics/ShaderUniformValue.cpp
        Graphics/ShaderUniformValue.h
        ShaderTypes.cpp
        Graphics/Resources/Buffer.cpp
        Graphics/Resources/Buffer.h
        RenderSceneTypes.h
        RenderSceneTypes.cpp
        Graphics/VertexDataTypes.cpp
        Graphics/MeshStorage.h
        Graphics/MeshStorage.cpp
        Graphics/PagedLinearBuffer.h
        Graphics/Slang/SlangFileSystem.cpp
        Graphics/Slang/SlangFileSystem.h
        Graphics/Slang/SlangBlob.cpp
        Graphics/Slang/SlangBlob.h
        Graphics/Slang/SlangType.h
        RenderSceneStorage.cpp
        RenderSceneStorage.h
        Graphics/ShaderCursor.cpp
        Graphics/ShaderCursor.h
        Graphics/ShaderBufferInterface.cpp
        Graphics/ShaderBufferInterface.h
        Graphics/GraphicsResourceManager.cpp
        Graphics/GraphicsResourceManager.h
        RenderingEnginePlatform.h
        Graphics/GraphicsPlatformTypes.h
        Graphics/GraphicsPlatformTypes.cpp
        RenderListener.cpp
        RenderListener.h
        Graphics/Slang/SlangCompiler.cpp
        Graphics/Slang/SlangCompiler.h
        Graphics/Resources/ImageSampler.cpp
        Graphics/Resources/ImageSampler.h
        Vendor/stb_image.cpp
        Graphics/StagingBuffer.cpp
        Graphics/StagingBuffer.h
        Graphics/Resources/ImageSamplerTypes.h
        Graphics/Resources/ImageSamplerTypes.cpp
        Vendor/stbimage.h
        Graphics/GraphicsResourceCache.cpp
        Graphics/GraphicsResourceCache.h
        Gizmos/Gizmos.cpp
        Gizmos/Gizmos.h
        MeshUtils.cpp
        MeshUtils.h
        Material.cpp
        Material.h
        RenderObjectView.cpp
        RenderObjectView.h
        Graphics/RenderFrameStats.cpp
        Graphics/RenderFrameStats.h
        2D/Tilemap/TileMap.cpp
        2D/Tilemap/TileMap.h
        2D/Tilemap/TileMapAtlas.cpp
        2D/Tilemap/TileMapAtlas.h
        2D/Renderer2D.cpp
        2D/Renderer2D.h
        RenderPasses/SimpleRenderPass.h
        RenderPasses/ClearRenderPass.cpp
        RenderPasses/ClearRenderPass.h
        Gizmos/GizmosRenderPass.cpp
        Gizmos/GizmosRenderPass.h
)

target_link_libraries(Rendering PUBLIC
        Core
)

target_compile_definitions(Rendering PUBLIC
        COCO_SERVICE_RENDERING
)

#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
 #   set(SLANG_BUILD_PRESET "debug")
#else()
#    set(SLANG_BUILD_PRESET "release")
#endif ()

#set(SLANG_LIB_TYPE STATIC CACHE STRING "" FORCE)
#set(SLANG_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
#set(SLANG_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
#set(SLANG_SLANG_LLVM_FLAVOR DISABLE CACHE STRING "" FORCE)
#set(SLANG_RHI_ENABLE_CPU OFF CACHE BOOL "" FORCE)
#set(SLANG_ENABLE_REPLAYER OFF CACHE BOOL "" FORCE)

#FetchContent_Declare(
#        slang
#        GIT_REPOSITORY https://github.com/shader-slang/slang.git
#        GIT_TAG v2026.5
#)

if(UNIX)
    set(SLANG_RELEASE_NAME "slang-2026.5-linux-x86_64.tar.gz")
elseif(WIN32)
    set(SLANG_RELEASE_NAME "slang-2026.5-windows-x86_64.zip")
else ()
    set(SLANG_RELEASE_NAME "slang-2026.5-macos-x86_64.tar.gz")
endif ()

FetchContent_Declare(
        slang
        URL https://github.com/shader-slang/slang/releases/download/v2026.5/${SLANG_RELEASE_NAME}
)

FetchContent_MakeAvailable(slang)

target_include_directories(Rendering PRIVATE
        ${slang_SOURCE_DIR}/include
)

target_link_libraries(Rendering PRIVATE
        ${slang_SOURCE_DIR}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}slang${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${slang_SOURCE_DIR}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}slang-compiler${CMAKE_SHARED_LIBRARY_SUFFIX}
)

FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
)

FetchContent_MakeAvailable(stb)

target_include_directories(Rendering PRIVATE
        ${stb_SOURCE_DIR}
)

#target_link_libraries(Rendering PRIVATE
#        ${slang_SOURCE_DIR}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}slang${CMAKE_SHARED_LIBRARY_SUFFIX}
#        ${SLANG_LIBRARIES}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}slang-compiler${CMAKE_SHARED_LIBRARY_SUFFIX}
#)

#ExternalProject_Add(
#        slang
#        GIT_REPOSITORY https://github.com/shader-slang/slang.git
#        GIT_TAG v2026.5
#        PREFIX ${THIRDPARTY_PREFIX_DIR}
#        BINARY_DIR ${THIRDPARTY_PREFIX_DIR}/src/slang-build
#        CMAKE_ARGS
#            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
#            -DSLANG_LIB_TYPE=STATIC
#            -DSLANG_ENABLE_TESTS=FALSE
#            -DSLANG_ENABLE_EXAMPLES=FALSE
#            -DSLANG_SLANG_LLVM_FLAVOR=DISABLE
#        BUILD_COMMAND ${CMAKE_COMMAND} --build --preset ${SLANG_BUILD_PRESET}
#)

#find_package(slang CONFIG REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)

if(COCO_RENDERING_INCLUDE_OPENGL)
    add_subdirectory(RHI/OpenGL)

    target_compile_definitions(Rendering PUBLIC
            COCO_RENDERING_OPENGL
    )

    find_package(OpenGL REQUIRED)

    target_include_directories(Rendering PRIVATE
            ${OPENGL_INCLUDE_DIRS}
            ${OPENGL_EGL_INCLUDE_DIRS}
            ${CMAKE_SOURCE_DIR}/src/Vendor/glad/include
    )

    target_link_libraries(Rendering PRIVATE
            ${OPENGL_opengl_LIBRARY}
            ${OPENGL_egl_LIBRARY}
    )

    target_sources(Rendering PRIVATE
            ${CMAKE_SOURCE_DIR}/src/Vendor/glad/src/glad.c
    )
endif()

if(COCO_RENDERING_INCLUDE_VULKAN)
    target_compile_definitions(Rendering PUBLIC
            COCO_RENDERING_VULKAN
    )

    add_subdirectory(RHI/Vulkan)
endif()

#set(SLANG_PREFIX ${CMAKE_SOURCE_DIR}/Vendor/slang)
#set(SLANG_INSTALL_PREFIX ${SLANG_PREFIX}/install)

# TODO: cross-platform support or source compile?
#set(SLANG_DIR ${CMAKE_SOURCE_DIR}/Vendor/slang/slang-2026.4.2-linux-x86_64)

#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#       set(SLANG_BUILD_PRESET "debug")
#else()
#    set(SLANG_BUILD_PRESET "release")
#endif ()

#ExternalProject_Add(slang
#        PREFIX ${SLANG_PREFIX}
#        SOURCE_DIR ${CMAKE_SOURCE_DIR}/Vendor/slang
#        INSTALL_DIR ${SLANG_INSTALL_PREFIX}
#        CMAKE_ARGS
#            -DCMAKE_INSTALL_PREFIX=${SLANG_INSTALL_PREFIX}
#            -DSLANG_LIB_TYPE=STATIC
#            -DSLANG_ENABLE_TESTS=FALSE
#            -DSLANG_ENABLE_EXAMPLES=FALSE
#            -DSLANG_SLANG_LLVM_FLAVOR=DISABLE
#        #BUILD_COMMAND ${CMAKE_COMMAND} --build --preset ${SLANG_BUILD_PRESET}
#        #INSTALL_COMMAND cmake --target install
#        #BUILD_BYPRODUCTS
#        #    ${SLANG_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}slang${CMAKE_STATIC_LIBRARY_SUFFIX}
#        #    ${SLANG_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}slang-compiler${CMAKE_STATIC_LIBRARY_SUFFIX}
#)

#add_dependencies(Rendering slang)
#set(SLANG_INCLUDE_DIR ${SLANG_DIR}/include)
#set(SLANG_LIBRARIES ${SLANG_DIR}/lib)

#target_include_directories(Rendering PRIVATE
#        ${SLANG_INCLUDE_DIR}
#)

#target_link_libraries(Rendering PRIVATE
#        ${SLANG_LIBRARIES}/${CMAKE_SHARED_LIBRARY_PREFIX}slang${CMAKE_SHARED_LIBRARY_SUFFIX}
#        ${SLANG_LIBRARIES}/${CMAKE_SHARED_LIBRARY_PREFIX}slang-compiler${CMAKE_SHARED_LIBRARY_SUFFIX}
#)