#include "ContentPanel.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/IO/FileSystems/UnpackedEngineFileSystem.h>
#include <filesystem>

#include <imgui.h>

namespace Coco
{
	void ContentPanel::Update(const TickInfo& tick)
	{
		if (ImGui::Begin("Content"))
		{
			if (UnpackedEngineFileSystem* fs = dynamic_cast<UnpackedEngineFileSystem*>(&Engine::Get()->GetFileSystem()))
			{
				FilePath basePath = fs->GetContentBasePath();
				DrawFileNode(basePath);
			}
		}

		ImGui::End();
	}

	void ContentPanel::DrawFileNode(const FilePath& path)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool isDirectory = path.IsDirectory();
		if (!isDirectory)
			flags |= ImGuiTreeNodeFlags_Leaf;

		string pathName = path.GetFileName(true);
		bool expanded = ImGui::TreeNodeEx(pathName.c_str(), flags, "%s", pathName.c_str());

		if (!isDirectory && ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptPeekOnly))
		{
			ImGui::SetDragDropPayload(path.GetExtension().c_str(), &path, sizeof(FilePath));
			ImGui::EndDragDropSource();
		}

		if (expanded)
		{
			if (isDirectory)
			{
				for (const auto& it : std::filesystem::directory_iterator{ path })
				{
					FilePath fp(it.path());
					DrawFileNode(fp);
				}
			}

			ImGui::TreePop();
		}
	}
}