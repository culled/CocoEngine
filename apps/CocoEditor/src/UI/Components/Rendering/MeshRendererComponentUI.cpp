#include "MeshRendererComponentUI.h"

#include <Coco/ECS/Components/Rendering/MeshRendererComponent.h>
#include <Coco/ECS/Components/Transform3DComponent.h>
#include <Coco/Rendering/Material.h>

#include <Coco/Rendering/Gizmos/GizmoRender.h>
#include "../../MaterialUI.h"
#include "../../UIUtils.h"

#include <Coco/Core/Engine.h>
#include <imgui.h>

using namespace Coco::ECS;
using namespace Coco::Rendering;

namespace Coco
{
	MeshRendererComponentUI::MeshRendererComponentUI() :
		_isAddingParam(false),
		_addParamUniformName(),
		_addParamUniformType(ShaderUniformType::Float),
		_isRemovingParam(false),
		_removeParamUniformName()
	{
		for (int i = 0; i < _flagTexts.size(); i++)
		{
			_flagTexts.at(i) = FormatString("Group {}", i);
		}

		for (int i = 0; i < _uniformTypeTexts.size(); i++)
		{
			ShaderUniformType u = static_cast<ShaderUniformType>(i);

			_uniformTypeTexts.at(i) = GetShaderUniformTypeString(u);
		}
	}

	void MeshRendererComponentUI::DrawPropertiesImpl(ECS::Entity& entity)
	{
		MeshRendererComponent& renderer = entity.GetComponent<MeshRendererComponent>();

		uint64 visibility = renderer.GetVisibilityGroups();
		string visibilityPreview = GetVisibilityFlagText(visibility, 2);

		if (ImGui::BeginCombo("Visibility Groups", visibilityPreview.c_str()))
		{
			for (int i = 0; i < _flagTexts.size(); i++)
			{
				const string& label = _flagTexts.at(i);
				uint64 flag = static_cast<uint64>(1) << i;
				bool selected = (visibility & flag) != 0;

				if (ImGui::Selectable(label.c_str(), &selected))
				{
					if (selected)
					{
						visibility |= flag;
					}
					else
					{
						visibility &= ~flag;
					}

					renderer.SetVisibilityGroups(visibility);
				}
			}

			ImGui::EndCombo();
		}

		SharedRef<Mesh> mesh = renderer.GetMesh();
		if (UIUtils::DrawResourcePicker(".cmesh", "Mesh", mesh))
		{
			renderer.SetMesh(mesh);
		}

		auto materials = renderer.GetMaterials();
		for (auto& it : materials)
		{
			ImGui::PushID(it.first);

			SharedRef<Material> materialResource = std::dynamic_pointer_cast<Material>(it.second);
			string t = FormatString("Material {}", it.first);
			UIUtils::DrawResourcePicker(".cmaterial", t.c_str(), materialResource);

			if (materialResource)
			{
				if (ImGui::CollapsingHeader("Properties"))
				{
					MaterialUI::Draw(*materialResource);
					DrawAddParameterSection(*materialResource);
					DrawRemoveParameterSection(*materialResource);

					//SharedRef<Shader> shader;
					//if (UIUtils::DrawResourcePicker(".cshader", "Add Parameters From Shader", shader))
					//{
					//	materialResource->AddParametersFromShader(*shader);
					//}
				}

				it.second = materialResource;
			}

			ImGui::PopID();
		}

		renderer.SetMaterials(materials);
	}

	void MeshRendererComponentUI::DrawGizmosImpl(ECS::Entity& entity, const SizeInt& viewportSize)
	{
		MeshRendererComponent& renderer = entity.GetComponent<MeshRendererComponent>();
		SharedRef<Mesh> mesh = renderer.GetMesh();
		if (!mesh)
			return;

		GizmoRender* gizmo = GizmoRender::Get();
		if (!gizmo)
			return;

		Matrix4x4 modelMatrix = Matrix4x4::Identity;

		if (entity.HasComponent<Transform3DComponent>())
		{
			Transform3DComponent& transform = entity.GetComponent<Transform3DComponent>();
			modelMatrix = transform.GetTransformMatrix(TransformSpace::Self, TransformSpace::Global);
		}

		gizmo->DrawWireBounds(mesh->GetBounds(), modelMatrix, Color::White);
	}

	string MeshRendererComponentUI::GetVisibilityFlagText(uint64 flags, int maxFlags)
	{
		string v;
		int added = 0;

		for (int i = 0; i < _flagTexts.size(); i++)
		{
			if ((flags & static_cast<uint64>(1) << i) != 0)
			{
				if (added < maxFlags)
				{
					if (added > 0)
						v += ", ";

					v += _flagTexts.at(i);
				}
				else
				{
					v += "...";
				}

				added++;
			}
		}

		if (added == 0)
			v = "None";

		return v;
	}

	void MeshRendererComponentUI::DrawAddParameterSection(Material& material)
	{
		if (ImGui::Button("Add Parameter..."))
		{
			_addParamUniformType = ShaderUniformType::Float;
			_addParamUniformName = "New Param";
			ImGui::OpenPopup("Add Material Parameter##1");
		}

		if (ImGui::BeginPopup("Add Material Parameter##1", ImGuiWindowFlags_AlwaysAutoResize))
		{
			_isAddingParam = true;

			UIUtils::DrawInputStringEdit("Name", _addParamUniformName, 64);

			const char* typePreview = GetShaderUniformTypeString(_addParamUniformType);

			if (ImGui::BeginCombo("Type", typePreview))
			{
				for (int i = 0; i < _uniformTypeTexts.size(); i++)
				{
					ShaderUniformType u = static_cast<ShaderUniformType>(i);
					const string& label = _uniformTypeTexts.at(i);
					bool selected = u == _addParamUniformType;

					if (ImGui::Selectable(label.c_str(), selected))
					{
						_addParamUniformType = u;
					}
				}

				ImGui::EndCombo();
			}

			if (!_addParamUniformName.empty() && !material.HasParameter(_addParamUniformName.c_str()))
			{
				if (ImGui::Button("Add"))
				{
					material.AddParameter(_addParamUniformName.c_str(), _addParamUniformType);
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (_isAddingParam && !ImGui::IsPopupOpen("Add Material Parameter##1"))
			_isAddingParam = false;
	}

	void MeshRendererComponentUI::DrawRemoveParameterSection(Rendering::Material& material)
	{
		std::vector<string> params;
		material.ForEachParameter(
			[&params](const MaterialParameter& param)
			{
				params.push_back(param.Name);
			}
		);

		if (params.size() > 0)
		{
			if (ImGui::Button("Remove Parameter..."))
			{
				_removeParamUniformName = params.front();
				ImGui::OpenPopup("Remove Material Parameter##1");
			}
		}

		if (ImGui::BeginPopup("Remove Material Parameter##1", ImGuiWindowFlags_AlwaysAutoResize))
		{
			_isRemovingParam = true;

			if (ImGui::BeginCombo("Name", _removeParamUniformName.c_str()))
			{
				for (int i = 0; i < params.size(); i++)
				{
					const string& label = params.at(i);
					bool selected = label == _removeParamUniformName;

					if (ImGui::Selectable(label.c_str(), selected))
					{
						_removeParamUniformName = label;
					}
				}

				ImGui::EndCombo();
			}

			if (ImGui::Button("Remove"))
			{
				material.RemoveParameter(_removeParamUniformName.c_str());
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (_isRemovingParam && !ImGui::IsPopupOpen("Remove Material Parameter##1"))
			_isRemovingParam = false;
	}
}