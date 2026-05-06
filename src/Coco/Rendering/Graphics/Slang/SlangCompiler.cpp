//
// Created by cullen on 3/25/26.
//

#include "SlangCompiler.h"

#include "Coco/Core/Asserts.h"
#include "Coco/Core/Engine.h"

namespace Coco
{
    SlangCompiler::SlangCompiler(SlangCompileTarget compileTarget, const char* profile)
    {
        SlangGlobalSessionDesc globalSessionDesc = {};

        auto result = slang::createGlobalSession(&globalSessionDesc, _globalSession.writeRef());
        COCO_ASSERT(SLANG_SUCCEEDED(result), "Failed to create Slang global session: %s", slang::getLastInternalErrorMessage());

        slang::SessionDesc sessionDesc = {};
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_ROW_MAJOR;

        const char* searchPath = "Assets";
        sessionDesc.searchPaths = &searchPath;
        sessionDesc.searchPathCount = 1;
        //sessionDesc.fileSystem = &_fileSystem;

        slang::TargetDesc targetDesc = {};
        targetDesc.format = compileTarget;
        targetDesc.profile = _globalSession->findProfile(profile);
        sessionDesc.targets = &targetDesc;
        sessionDesc.targetCount = 1;

        StackArray<slang::CompilerOptionEntry, 1> compilerOptions = {
            slang::CompilerOptionEntry(slang::CompilerOptionName::VulkanUseEntryPointName, {slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr})
        };
        sessionDesc.compilerOptionEntries = compilerOptions.Data();
        sessionDesc.compilerOptionEntryCount = compilerOptions.GetCount();

        result = _globalSession->createSession(sessionDesc, _session.writeRef());
        COCO_ASSERT(SLANG_SUCCEEDED(result), "Failed to create Slang session: %s", slang::getLastInternalErrorMessage());
    }

    SlangCompiler::~SlangCompiler()
    {
        _session.setNull();
        _globalSession.setNull();
        slang::shutdown();
    }

    void SlangCompiler::ReflectVertexAttributes(slang::ProgramLayout* programLayout,
        ArrayContainer<VertexChannel>& outVertexChannels)
    {
        slang::EntryPointReflection* entryPointReflection = nullptr;
        for (uint8 i = 0; i < programLayout->getEntryPointCount(); i++)
        {
            auto entryPoint = programLayout->getEntryPointByIndex(i);
            if (entryPoint->getStage() == SLANG_STAGE_VERTEX)
            {
                entryPointReflection = entryPoint;
                break;
            }
        }

        COCO_ASSERT(entryPointReflection, "Failed to find vertex shader entry point");

        for (long i = 0; i < entryPointReflection->getParameterCount(); i++)
        {
            auto param = entryPointReflection->getParameterByIndex(i);
            auto category = param->getCategory();

            if (category != slang::ParameterCategory::VaryingInput)
                continue;

            String semanticName(param->getSemanticName());

            if (semanticName.Contains("POSITION"))
            {
                outVertexChannels.Append(VertexChannel::Position);
            }
            else if (semanticName.Contains("NORMAL"))
            {
                outVertexChannels.Append(VertexChannel::Normal);
            }
            else if (semanticName.Contains("TEXCOORD"))
            {
                outVertexChannels.Append(VertexChannel::UV0);
            }
            else if (semanticName.Contains("COLOR"))
            {
                outVertexChannels.Append(VertexChannel::Color);
            }
            else if (semanticName.Contains("TANGENT"))
            {
                outVertexChannels.Append(VertexChannel::Tangent);
            }
            else
            {
                COCO_ENGINE_LOG_ERROR("Unknown vertex input semantic: %s", semanticName.CStr());
            }
        }
    }

    Slang::ComPtr<slang::IComponentType> SlangCompiler::CompileShader(const FilePath& shaderFile)
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

    void SlangCompiler::PrintDiagnostics(slang::IBlob* diagnostics)
    {
        if (!diagnostics)
            return;

        COCO_ENGINE_LOG_INFO("%s", static_cast<const char*>(diagnostics->getBufferPointer()));
    }
} // Coco