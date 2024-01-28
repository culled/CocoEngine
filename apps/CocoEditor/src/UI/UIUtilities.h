#pragma once

#include <Coco/Core/Types/String.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Resources/Resource.h>
#include <Coco/Core/Engine.h>

#include <imgui.h>

namespace Coco
{
	struct UIUtilities
	{
		static bool DrawInputStringEdit(uint64 id, const char* label, string& str, int maxLength);
		static bool DrawVector3UI(uint64 id, const char* label, Vector3& vector, double resetValue = 0.0);

		template<typename ResourceType>
		static bool DrawResourcePicker(const char* resourceTypename, const char* label, SharedRef<ResourceType>& resource)
		{
			string pathText = "None";

			if (resource)
			{
				pathText = resource->GetID().ToString();
			}

			ImGui::Text("%s", label);
			ImGui::SameLine();
			ImGui::Button(pathText.c_str());

			AssetManager& assets = Engine::Get()->GetAssetManager();

			bool changed = false;

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(resourceTypename))
				{
					string resourcePath(static_cast<const char*>(payload->Data), payload->DataSize);
					resource = assets.GetOrLoadAs<ResourceType>(resourcePath);
					changed = true;
				}

				ImGui::EndDragDropTarget();
			}

			return changed;
		}
	};
}