#include "ComponentUI.h"

#include <Coco/ECS/Components/EntityInfoComponent.h>
#include "../ComponentUI/EntityInfoComponentUI.h"

#include <Coco/ECS/Components/Transform3DComponent.h>
#include "../ComponentUI/Transform3DComponentUI.h"

#include <Coco/ECS/Components/Rendering/CameraComponent.h>
#include "../ComponentUI/Rendering/CameraComponentUI.h"

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

	void ComponentUI::DrawProperties(ECS::Entity& entity)
	{
		for (const auto& ui : _sComponentUIInstances)
		{
			if (!ui.TestFunc(entity))
				continue;

			if (ImGui::CollapsingHeader(ui.UI->GetHeader(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				ui.UI->DrawPropertiesImpl(entity);
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