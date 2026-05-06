//
// Created by cullen on 3/10/26.
//

#include "OpenGLShaderProgram.h"

#include "Coco/Rendering/RHI/OpenGL/OpenGLGraphicsPlatform.h"

#include "Coco/Core/Engine.h"
#include "Coco/Core/IO/File.h"
#include "Coco/Rendering/RHI/OpenGL/OpenGLShaderProgramCompiler.h"
#include "Coco/Rendering/RHI/OpenGL/OpenGLUtils.h"
#include <glad/glad.h>

namespace Coco
{
    OpenGLShaderProgram::OpenGLShaderProgram(OpenGLGraphicsPlatform* platform, uint64 id, const FilePath& shaderPath) :
        ShaderProgram(id),
        _platform(platform),
        _programID(0),
        _isReady(false)
    {
        _linkedProgram = _platform->GetShaderCompiler()->CompileShader(shaderPath);
        _typeLayoutInfo = _linkedProgram->getLayout()->getGlobalParamsTypeLayout();
        CreateProgram();

        _isReady = true;
    }

    OpenGLShaderProgram::~OpenGLShaderProgram()
    {
        if (_programID)
        {
            glDeleteProgram(_programID);
            _programID = 0;
        }
    }

    slang::TypeLayoutReflection* OpenGLShaderProgram::GetParamBlockLayout(const char* name)
    {
        long index = _typeLayoutInfo->findFieldIndexByName(name);
        COCO_ASSERT(index != -1, "Failed to find field %s", name);

        auto field = _typeLayoutInfo->getFieldByIndex(index);
        return field->getTypeLayout()->getElementTypeLayout();
    }

    void OpenGLShaderProgram::Bind()
    {
        glUseProgram(_programID);
    }

    uint32 OpenGLShaderProgram::CreateShader(ShaderStageType stage, uint8 entryPointIndex)
    {
        Slang::ComPtr<slang::IBlob> shaderCode;
        SlangResult result = _linkedProgram->getEntryPointCode(entryPointIndex, 0, shaderCode.writeRef());

        if (SLANG_FAILED(result))
            throw Exception("Failed to get program code");

        GLuint glShader = glCreateShader(OpenGLUtils::ToShaderStage(stage));
        const GLchar* str = static_cast<const char*>(shaderCode->getBufferPointer());
        glShaderSource(glShader, 1, &str, nullptr);
        glCompileShader(glShader);

        //const char* entryPointName = entryPointIndex == 0 ? "vsMain" : "psMain";
        //glShaderBinary(1, &glShader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, shaderCode->getBufferPointer(), static_cast<GLsizei>(shaderCode->getBufferSize()));
        //glSpecializeShaderARB(glShader, entryPointName, 0, nullptr, nullptr);

        int success;
        char infoLog[512];
        glGetShaderiv(glShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(glShader, 512, nullptr, infoLog);
            COCO_ASSERT(false, "Error compiling shader: %s", infoLog);
        }

        return glShader;
    }

    void OpenGLShaderProgram::CreateProgram()
    {
        uint32 vertexShader = CreateShader(ShaderStageType::Vertex, 0);
        uint32 fragmentShader = CreateShader(ShaderStageType::Fragment, 1);
        /*Slang::ComPtr<slang::IBlob> shaderCode;
        SlangResult result = _linkedProgram->getTargetCode(0, shaderCode.writeRef());

        if (SLANG_FAILED(result))
            throw Exception("Failed to get program code");

        auto file = Engine::Get()->GetFileSystem()->Open("shader.spirv", FileOpenFlags::Write, false);
        file.Write(Span<const uint8>(static_cast<const uint8*>(shaderCode->getBufferPointer()), shaderCode->getBufferSize()));
        file.Close();

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        GLuint shaders[] = { vertexShader, fragmentShader };

        glShaderBinary(2, shaders, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, shaderCode->getBufferPointer(), static_cast<GLsizei>(shaderCode->getBufferSize()));
        glSpecializeShaderARB(vertexShader, "vsMain", 0, nullptr, nullptr);
        glSpecializeShaderARB(fragmentShader, "psMain", 0, nullptr, nullptr);

        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            COCO_ASSERT(false, "Error compiling vertex shader: %s", infoLog);
        }

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            COCO_ASSERT(false, "Error compiling fragment shader: %s", infoLog);
        }*/

        _programID = glCreateProgram();
        glAttachShader(_programID, vertexShader);
        glAttachShader(_programID, fragmentShader);

        glLinkProgram(_programID);

        int success;
        char infoLog[512];
        glGetProgramiv(_programID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(_programID, 512, nullptr, infoLog);
            COCO_ASSERT(false, "Error linking shader: %s", infoLog);
        }

        glDetachShader(_programID, vertexShader);
        glDetachShader(_programID, fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
} // Coco