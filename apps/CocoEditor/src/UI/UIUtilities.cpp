#include "UIUtilities.h"

#include <imgui.h>

namespace Coco
{
	bool UIUtilities::DrawInputStringEdit(uint64 id, const char* label, string& str, int maxLength)
	{
		ImGui::PushID(static_cast<int>(id));

		size_t size = str.size();
		str.resize(maxLength);

		bool changed = ImGui::InputText(label, str.data(), maxLength);

		if (changed)
		{
			size = str.find_first_of('\0');
		}

		str.resize(size);

		ImGui::PopID();

		return changed;
	}

	bool UIUtilities::DrawVector3UI(uint64 id, const char* label, Vector3& vector, double resetValue)
	{
		ImGui::PushID(static_cast<int>(id));

		ImGui::Text(label);

		ImGui::BeginTable(label, 6, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX);

		bool changed = false;
		const float buttonWidth = 16.f;
		const char* numberFormat = "%.3f";
		const float sliderDragSpeed = 0.1f;
		const ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_NoRoundToFormat;

		ImVec2 size = ImGui::GetContentRegionAvail();
		float valueWidth = (size.x - buttonWidth * 3.f) / 3.f;

		ImGui::SetNextItemWidth(buttonWidth);
		ImGui::TableNextColumn();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.f));

		if (ImGui::Button("X"))
		{
			vector.X = resetValue;
			changed = true;
		}

		ImGui::PopStyleColor(3);

		ImGui::SetNextItemWidth(valueWidth);
		ImGui::TableNextColumn();

		float x = static_cast<float>(vector.X);
		if (ImGui::DragFloat("##X", &x, sliderDragSpeed, 0.f, 0.f, numberFormat, sliderFlags))
		{
			vector.X = x;
			changed = true;
		}

		ImGui::SetNextItemWidth(buttonWidth);
		ImGui::TableNextColumn();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.3f, 1.f));

		if (ImGui::Button("Y"))
		{
			vector.Y = resetValue;
			changed = true;
		}

		ImGui::PopStyleColor(3);

		ImGui::SetNextItemWidth(valueWidth);
		ImGui::TableNextColumn();

		float y = static_cast<float>(vector.Y);
		if (ImGui::DragFloat("##Y", &y, sliderDragSpeed, 0.f, 0.f, numberFormat, sliderFlags))
		{
			vector.Y = y;
			changed = true;
		}

		ImGui::SetNextItemWidth(buttonWidth);
		ImGui::TableNextColumn();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.3f, 0.9f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.f));

		if (ImGui::Button("Z"))
		{
			vector.Z = resetValue;
			changed = true;
		}

		ImGui::PopStyleColor(3);

		ImGui::SetNextItemWidth(valueWidth);
		ImGui::TableNextColumn();

		float z = static_cast<float>(vector.Z);
		if (ImGui::DragFloat("##Z", &z, sliderDragSpeed, 0.f, 0.f, numberFormat, sliderFlags))
		{
			vector.Z = z;
			changed = true;
		}

		ImGui::EndTable();

		ImGui::PopID();

		return changed;
	}
}