#pragma once
#include "ResourceDialog.h"
#include <Coco/Core/IO/FileTypes.h>
#include <Coco/Core/Types/Vector.h>

namespace Coco::Rendering
{
    class Mesh;
}

namespace Coco
{
    class MeshPrimitiveResourceDialog :
        public ResourceDialog
    {
    private:
        FilePath _savePath;
        string _newResourceName;
        int _newResourceType;
        Vector3 _newResourceSize;
        bool _newResourceFlip;
        Vector2Int _newResourceSphereOptions;

    public:
        MeshPrimitiveResourceDialog();

        void SetSavePath(const FilePath& savePath);

    protected:
        void Opened() override;
        const char* GetPopupName() const override { return "Create Mesh Primitive"; }
        void Draw() override;

    private:
        static const char* GetPrimitiveNameFromType(int type);

        void DrawQuadOptions();
        void DrawCubeOptions();
        void DrawSphereOptions();

        void SetMeshData(SharedRef<Rendering::Mesh> mesh);
    };
}