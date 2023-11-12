#include "MaterialResourceDialog.h"

#include "../UI/UIUtils.h"
#include <Coco/Rendering/Material.h>

using namespace Coco::Rendering;

namespace Coco
{
	MaterialResourceDialog::MaterialResourceDialog() :
		_savePath(),
		_newMaterialName()
	{}

	void MaterialResourceDialog::SetSavePath(const FilePath& savePath)
	{
		_savePath = savePath;
	}

	void MaterialResourceDialog::Opened()
	{
		_newMaterialName = "New Material";
	}

	void MaterialResourceDialog::Draw()
	{
		UIUtils::DrawInputStringEdit("Resource Name", _newMaterialName, 64);

		if (ImGui::Button("Create") && !_newMaterialName.empty())
		{
			auto& resources = Engine::Get()->GetResourceLibrary();
			SharedRef<Material> material = resources.Create<Material>(_newMaterialName);
			string fileName = _newMaterialName + ".cmaterial";
			resources.Save(FilePath::CombineToPath(_savePath, fileName), material, false);

			Close();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
			Close();
	}
}