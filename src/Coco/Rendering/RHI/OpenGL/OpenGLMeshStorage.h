//
// Created by cullen on 3/14/26.
//

#ifndef COCOENGINE_OPENGLMESHSTORAGE_H
#define COCOENGINE_OPENGLMESHSTORAGE_H
#include "Coco/Core/Types/StackArray.h"
#include "Coco/Rendering/Mesh.h"
#include "Coco/Rendering/Graphics/MeshStorage.h"

namespace Coco
{
    class OpenGLGraphicsPlatform;
    class OpenGLBuffer;

    struct OpenGLMeshBuffer
    {
        Ref<OpenGLBuffer> MeshBuffer;
        StackArray<uint64, 5> ChannelOffsets;
        VertexChannelFlags VertexChannels;
        uint64 VertexDataSize;
        uint64 IndexDataOffset;
        uint64 IndexCount;
        uint32 VertexArrayHandle;

        OpenGLMeshBuffer(Ref<OpenGLBuffer> meshBuffer);
        ~OpenGLMeshBuffer();

        void UpdateVertexArray();
    };

    class OpenGLMeshStorage : public MeshStorage
    {
    public:
        OpenGLMeshStorage(OpenGLGraphicsPlatform* platform);
        ~OpenGLMeshStorage();

        void AddMesh(Mesh& mesh) override;
        void RemoveMesh(uint64 meshID) override;
        void ClearFrameMeshBuffers(uint8 frameIndex);

        const OpenGLMeshBuffer* GetMeshBuffer(uint64 meshID) const;

    private:
        OpenGLGraphicsPlatform* _platform;
        Map<uint64, OpenGLMeshBuffer> _staticMeshBuffers;
        Array<Map<uint64, OpenGLMeshBuffer>> _frameMeshBuffers;

    private:
        Map<uint64, OpenGLMeshBuffer>* GetMeshBuffers(bool dynamic);
    };
} // Coco

#endif //COCOENGINE_OPENGLMESHSTORAGE_H