//
// Created by cullen on 3/19/26.
//

#include "OpenGLShaderProgramCompiler.h"

#include "OpenGLUtils.h"
#include "Coco/Core/Engine.h"
#include "Coco/Core/Types/StackArray.h"
#include "Coco/Rendering/Graphics/VertexDataTypes.h"
#include "glad/glad.h"

namespace Coco
{
    OpenGLShaderProgramCompiler::OpenGLShaderProgramCompiler() :
        _globalSession(),
        _session(),
        _fileSystem()
    {
        SlangGlobalSessionDesc globalSessionDesc = {};

        auto result = slang::createGlobalSession(&globalSessionDesc, _globalSession.writeRef());
        COCO_ASSERT(SLANG_SUCCEEDED(result), "Failed to create Slang global session: %s", slang::getLastInternalErrorMessage());

        slang::SessionDesc sessionDesc = {};

        const char* searchPath = "Assets";
        sessionDesc.searchPaths = &searchPath;
        sessionDesc.searchPathCount = 1;
        //sessionDesc.fileSystem = &_fileSystem;

        slang::TargetDesc targetDesc = {};
        targetDesc.format = SLANG_GLSL;
        targetDesc.profile = _globalSession->findProfile("glsl_450");
        //targetDesc.format = SLANG_SPIRV;
        //targetDesc.profile = _globalSession->findProfile("spirv_1_5");
        sessionDesc.targets = &targetDesc;
        sessionDesc.targetCount = 1;

        //StackArray<slang::CompilerOptionEntry, 1> compilerOptions = {
        //    slang::CompilerOptionEntry(slang::CompilerOptionName::VulkanUseEntryPointName, {slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr})
        //};
        //sessionDesc.compilerOptionEntries = compilerOptions.Data();
        //sessionDesc.compilerOptionEntryCount = compilerOptions.GetCount();
        sessionDesc.compilerOptionEntryCount = 0;

        result = _globalSession->createSession(sessionDesc, _session.writeRef());
        COCO_ASSERT(SLANG_SUCCEEDED(result), "Failed to create Slang session: %s", slang::getLastInternalErrorMessage());
    }

    OpenGLShaderProgramCompiler::~OpenGLShaderProgramCompiler()
    {
        _session.setNull();
        _globalSession.setNull();
        slang::shutdown();
    }

    Slang::ComPtr<slang::IComponentType> OpenGLShaderProgramCompiler::CompileShader(const FilePath& shaderFile)
    {
        COCO_ENGINE_LOG_VERBOSE("Loading and compiling shader \"%s\"", shaderFile.CStr());

        FileSystem* fs = Engine::Get()->GetFileSystem();
        File file = fs->Open(shaderFile, FileOpenFlags::Read, false);
        String fileText;
        file.ReadTextToEnd(fileText);
        file.Close();

        String shaderName = shaderFile.GetFileName(false);
        Slang::ComPtr<slang::IBlob> diagnostics;
        Slang::ComPtr<slang::IModule> module;
        module = _session->loadModuleFromSourceString(shaderName.CStr(), shaderFile.CStr(), fileText.CStr(), diagnostics.writeRef());
        PrintDiagnostics(diagnostics);

        if (!module)
            throw Exception("Failed to load module");

        Slang::ComPtr<slang::IEntryPoint> vertexEntryPoint;
        module->findEntryPointByName("vsMain", vertexEntryPoint.writeRef());
        if (!vertexEntryPoint)
            throw Exception("Failed to find vertex entry point");

        Slang::ComPtr<slang::IEntryPoint> fragmentEntryPoint;
        module->findEntryPointByName("psMain", fragmentEntryPoint.writeRef());
        if (!fragmentEntryPoint)
            throw Exception("Failed to find fragment entry point");

        StackArray<slang::IComponentType*, 3> components = {
            module,
            vertexEntryPoint,
            fragmentEntryPoint
        };

        Slang::ComPtr<slang::IComponentType> composedProgram;
        SlangResult result = _session->createCompositeComponentType(components.Data(), static_cast<uint32>(components.GetCount()), composedProgram.writeRef(), diagnostics.writeRef());
        PrintDiagnostics(diagnostics);

        if (SLANG_FAILED(result))
            throw Exception("Failed to create composite component type");

        Slang::ComPtr<slang::IComponentType> linkedProgram;
        result = composedProgram->link(linkedProgram.writeRef(), diagnostics.writeRef());
        PrintDiagnostics(diagnostics);

        if (SLANG_FAILED(result))
            throw Exception("Failed to link program type");

        return linkedProgram;
    }

    void OpenGLShaderProgramCompiler::PrintDiagnostics(slang::IBlob* diagnostics)
    {
        if (!diagnostics)
            return;

        COCO_ENGINE_LOG_INFO("%s", static_cast<const char*>(diagnostics->getBufferPointer()));
    }
} // Coco