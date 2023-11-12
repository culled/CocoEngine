#pragma once
#include "ResourceDialog.h"
#include <Coco/Core/IO/FileTypes.h>

namespace Coco
{
    class MaterialResourceDialog :
        public ResourceDialog
    {
    private:
        FilePath _savePath;
        string _newMaterialName;

    public:
        MaterialResourceDialog();

        void SetSavePath(const FilePath& savePath);

    protected:
        void Opened() override;
        const char* GetPopupName() const override { return "Create Material"; }
        void Draw() override;
    };
}