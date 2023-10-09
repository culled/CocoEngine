#include "ComponentUI.h"

#include <Coco/ECS/Components/EntityInfoComponent.h>
#include "Components/EntityInfoComponentUI.h"

#include <Coco/ECS/Components/Transform3DComponent.h>
#include "Components/Transform3DComponentUI.h"

#include <Coco/ECS/Components/Rendering/CameraComponent.h>
#include "Components/Rendering/CameraComponentUI.h"

#include <imgui.h>

using namespace Coco::ECS;

namespace Coco
{
	std::vector<ComponentUIInstance> ComponentUI::_sComponentUIInstances = std::vector<ComponentUIInstance>();

	void ComponentUI::RegisterBuiltInComponentUIs()
	{
		RegisterComponentUI<EntityInfoComponent, EntityInfoComponentUI>();
		RegisterComponentUI<Transform3DComponent, Transform3DComponentUI>();
		RegisterComponentUI<CameraComponent, CameraComponentUI>();
	}

	bool ComponentUI::DrawAddComponentUI(ECS::Entity& entity)
	{
		for (ComponentUIInstance& inst : _sComponentUIInstances)
		{
			if (inst.TestFunc(entity))
				continue;

			if (ImGui::MenuItem(inst.UI->GetHeader()))
			{
				inst.AddFunc(entity);
				return true;
			}
		}

		return false;
	}

	void ComponentUI::DrawProperties(ECS::Entity& entity)
	{
		for (const auto& ui : _sComponentUIInstances)
		{
			if (!ui.TestFunc(entity))
				continue;

			bool open = ImGui::CollapsingHeader(ui.UI->GetHeader(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);

			bool remove = false;

			if (ui.UI->IsRemovable())
			{
				const float buttonWidth = 30.f;
				ImGui::SameLine(ImGui::GetWindowWidth() - buttonWidth);

				if (ImGui::Button("...", ImVec2{ buttonWidth, 18.f }))
				{
					ImGui::OpenPopup("ComponentSettings");
				}

				if (ImGui::BeginPopup("ComponentSettings"))
				{
					if (ImGui::MenuItem("Remove"))
					{
						remove = true;
					}

					ImGui::EndPopup();
				}
			}

			if (open)
			{
				ui.UI->DrawPropertiesImpl(entity);
			}

			if (remove)
			{
				ui.RemoveFunc(entity);
			}
		}
	}

	void ComponentUI::DrawGizmos(ECS::Entity& entity, const SizeInt& viewportSize)
	{
		for (const auto& ui : _sComponentUIInstances)
		{
			if (!ui.TestFunc(entity))
				continue;

			ui.UI->DrawGizmosImpl(entity, viewportSize);
		}
	}
}