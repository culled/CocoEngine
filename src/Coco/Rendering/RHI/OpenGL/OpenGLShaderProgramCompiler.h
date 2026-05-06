//
// Created by cullen on 3/19/26.
//

#ifndef COCOENGINE_OPENGLSHADERPROGRAMCOMPILER_H
#define COCOENGINE_OPENGLSHADERPROGRAMCOMPILER_H

#include "Coco/Rendering/Graphics/Slang/SlangFileSystem.h"

#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>

namespace Coco
{
    class OpenGLShaderProgramCompiler
    {
    public:
        OpenGLShaderProgramCompiler();
        ~OpenGLShaderProgramCompiler();

        Slang::ComPtr<slang::IComponentType> CompileShader(const FilePath& shaderFile);

    private:
        Slang::ComPtr<slang::IGlobalSession> _globalSession;
        Slang::ComPtr<slang::ISession> _session;
        SlangFileSystem _fileSystem;

    private:
        static void PrintDiagnostics(slang::IBlob* diagnostics);
    };
} // Coco

#endif //COCOENGINE_OPENGLSHADERPROGRAMCOMPILER_H