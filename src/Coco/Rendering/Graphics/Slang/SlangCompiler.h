//
// Created by cullen on 3/25/26.
//

#ifndef COCOENGINE_SLANGCOMPILER_H
#define COCOENGINE_SLANGCOMPILER_H

#include <Coco/Core/IO/FilePath.h>

#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>

#include "Coco/Core/Types/ArrayContainer.h"
#include "Coco/Rendering/ShaderTypes.h"
#include "Coco/Rendering/Graphics/VertexDataTypes.h"

namespace Coco
{
    class SlangCompiler
    {
    public:
        SlangCompiler(SlangCompileTarget compileTarget, const char* profile);
        ~SlangCompiler();

        static void ReflectVertexAttributes(slang::ProgramLayout* programLayout, ArrayContainer<VertexChannel>& outVertexChannels);

        Slang::ComPtr<slang::IComponentType> CompileShader(const FilePath& shaderFile);

    private:
        Slang::ComPtr<slang::IGlobalSession> _globalSession;
        Slang::ComPtr<slang::ISession> _session;

    private:
        static void PrintDiagnostics(slang::IBlob* diagnostics);
    };
} // Coco

#endif //COCOENGINE_SLANGCOMPILER_H