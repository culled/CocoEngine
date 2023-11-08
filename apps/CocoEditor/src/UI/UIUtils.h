#pragma once

#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/String.h>

#include <Coco/Core/Engine.h>
#include <imgui.h>

namespace Coco
{
	struct UIUtils
	{
		static bool DrawVector3UI(uint64 id, const char* label, Vector3& vector, double resetValue = 0.0);

		template<typename ResourceType>
		static bool DrawResourcePicker(const char* resourceExtension, const char* label, SharedRef<ResourceType>& resource)
		{
			string pathText = "None";

			if(resource)
			{
				const FilePath& fp = resource->GetContentPath();
				
				if (!fp.IsEmpty())
					pathText = fp.ToString();
			}

			string text = FormatString("{}: {}", label, pathText);
			ImGui::Button(text.c_str());

			auto& resources = Engine::Get()->GetResourceLibrary();

			bool changed = false;

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(resourceExtension))
				{
					string resourcePath(static_cast<const char*>(payload->Data), payload->DataSize);
					resource = resources.GetOrLoad<ResourceType>(resourcePath);
					changed = true;
				}

				ImGui::EndDragDropTarget();
			}

			return changed;
		}
	};
}