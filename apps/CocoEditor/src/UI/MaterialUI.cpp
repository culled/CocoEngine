#include "MaterialUI.h"
#include "UIUtilities.h"

#include <imgui.h>

using namespace Coco::Rendering;

namespace Coco
{
	void MaterialUI::Draw(Material& material)
	{
		bool materialValueChanged = false;

		for (auto& uniform : material.GetUniformValues())
		{
			ImGui::PushID(uniform.Name.c_str());

			const char* name = uniform.Name.c_str();

			switch (uniform.Type)
			{
			case ShaderUniformType::Float:
			{
				float v = uniform.As<float>();
				if (ImGui::DragFloat(name, &v, 0.1f))
				{
					uniform.Value = v;
					materialValueChanged = true;
				}
				break;
			}
			case ShaderUniformType::Float2:
			{
				Float2 v = uniform.As<Float2>();
				if (ImGui::DragFloat2(name, v.data(), 0.1f))
				{
					uniform.Value = v;
					materialValueChanged = true;
				}
				break;
			}
			case ShaderUniformType::Float3:
			{
				Float3 v = uniform.As<Float3>();
				if (ImGui::DragFloat3(name, v.data(), 0.1f))
				{
					uniform.Value = v;
					materialValueChanged = true;
				}
				break;
			}
			case ShaderUniformType::Float4:
			{
				Float4 v = uniform.As<Float4>();
				if (ImGui::DragFloat4(name, v.data(), 0.1f))
				{
					uniform.Value = v;
					materialValueChanged = true;
				}
				break;
			}
			case ShaderUniformType::Color:
			{
				Float4 v = uniform.As<Float4>();
				if (ImGui::ColorEdit4(name, v.data()))
				{
					Color c(v, false);
					uniform = ShaderUniformValue(uniform.Name, c);
					materialValueChanged = true;
				}
				break;
			}
			case ShaderUniformType::Bool:
			{
				bool v = uniform.As<bool>();
				if (ImGui::Checkbox(name, &v))
				{
					uniform.Value = v;
					materialValueChanged = true;
				}
				break;
			}
			case ShaderUniformType::Int:
			{
				int v = uniform.As<int>();
				if (ImGui::DragInt(name, &v))
				{
					uniform.Value = v;
					materialValueChanged = true;
				}
				break;
			}
			case ShaderUniformType::Int2:
			{
				Int2 v = uniform.As<Int2>();
				if (ImGui::DragInt2(name, v.data()))
				{
					uniform.Value = v;
					materialValueChanged = true;
				}
				break;
			}
			case ShaderUniformType::Int3:
			{
				Int3 v = uniform.As<Int3>();
				if (ImGui::DragInt3(name, v.data()))
				{
					uniform.Value = v;
					materialValueChanged = true;
				}
				break;
			}
			case ShaderUniformType::Int4:
			{
				Int4 v = uniform.As<Int4>();
				if (ImGui::DragInt4(name, v.data()))
				{
					uniform.Value = v;
					materialValueChanged = true;
				}
				break;
			}
			case ShaderUniformType::Texture:
			{
				SharedRef<Texture> tex = uniform.As<SharedRef<Texture>>();
				if (UIUtilities::DrawResourcePicker("Texture", name, tex))
				{
					uniform.Value = tex;
					materialValueChanged = true;
				}

				break;
			}
			case ShaderUniformType::Matrix4x4:
			{
				FloatMatrix4x4 v = uniform.As<FloatMatrix4x4>();
				bool changed = false;

				if (ImGui::DragFloat4("", v.data()))
				{
					changed = true;
				}

				if (ImGui::DragFloat4("", v.data() + 4))
				{
					changed = true;
				}

				if (ImGui::DragFloat4("", v.data() + 8))
				{
					changed = true;
				}

				if (ImGui::DragFloat4("", v.data() + 12))
				{
					changed = true;
				}

				if (changed)
				{
					uniform.Value = v;
					materialValueChanged = true;
				}
				break;
			}
			default:
				break;
			}

			ImGui::PopID();
		}

		if (materialValueChanged)
			material.MarkDirty();
	}
}