#include "MaterialUI.h"

#include "UIUtils.h"

using namespace Coco::Rendering;

namespace Coco
{
	void MaterialUI::Draw(Material& material)
	{
		int i = 0;
		material.ForEachParameter(
			[&i, &material](const MaterialParameter& param)
			{
				ImGui::PushID(i);

				const char* name = param.Name.c_str();

				switch (param.Type)
				{
				case ShaderUniformType::Float:
				{
					float v = param.As<float>();
					if (ImGui::DragFloat(name, &v, 0.1f))
					{
						material.SetValue(name, v);
					}
					break;
				}
				case ShaderUniformType::Float2:
				{
					Vector2 v = param.As<Vector2>();
					std::array<float, 2> vf = v.AsFloatArray();
					if (ImGui::DragFloat2(name, vf.data(), 0.1f))
					{
						material.SetValue(name, Vector2(vf));
					}
					break;
				}
				case ShaderUniformType::Float3:
				{
					Vector3 v = param.As<Vector3>();
					std::array<float, 3> vf = v.AsFloatArray();
					if (ImGui::DragFloat3(name, vf.data(), 0.1f))
					{
						material.SetValue(name, Vector3(vf));
					}
					break;
				}
				case ShaderUniformType::Float4:
				{
					Vector4 v = param.As<Vector4>();
					std::array<float, 4> vf = v.AsFloatArray();
					if (ImGui::DragFloat4(name, vf.data(), 0.1f))
					{
						material.SetValue(name, Vector4(vf));
					}
					break;
				}
				case ShaderUniformType::Color:
				{
					Color v = param.As<Color>();
					std::array<float, 4> vf = v.AsFloatArray(false);
					if (ImGui::ColorEdit4(name, vf.data()))
					{
						v.R = vf[0];
						v.G = vf[1];
						v.B = vf[2];
						v.A = vf[3];
						material.SetValue(name, v);
					}
					break;
				}
				case ShaderUniformType::Bool:
				{
					bool v = param.As<bool>();
					if (ImGui::Checkbox(name, &v))
					{
						material.SetValue(name, v);
					}
					break;
				}
				case ShaderUniformType::Int:
				{
					int v = param.As<int>();
					if (ImGui::DragInt(name, &v))
					{
						material.SetValue(name, v);
					}
					break;
				}
				case ShaderUniformType::Int2:
				{
					Vector2Int v = param.As<Vector2Int>();
					std::array<int, 2> vf = v.AsIntArray();
					if (ImGui::DragInt2(name, vf.data()))
					{
						material.SetValue(name, Vector2Int(vf));
					}
					break;
				}
				case ShaderUniformType::Int3:
				{
					Vector3Int v = param.As<Vector3Int>();
					std::array<int, 3> vf = v.AsIntArray();
					if (ImGui::DragInt3(name, vf.data()))
					{
						material.SetValue(name, Vector3Int(vf));
					}
					break;
				}
				case ShaderUniformType::Int4:
				{
					Vector4Int v = param.As<Vector4Int>();
					std::array<int, 4> vf = v.AsIntArray();
					if (ImGui::DragInt4(name, vf.data()))
					{
						material.SetValue(name, Vector4Int(vf));
					}
					break;
				}
				case ShaderUniformType::Texture:
				{
					SharedRef<Texture> tex = param.As<SharedRef<Texture>>();
					if (UIUtils::DrawResourcePicker(".ctexture", name, tex))
						material.SetValue(name, tex);

					break;
				}
				case ShaderUniformType::Mat4x4:
				{
					Matrix4x4 v = param.As<Matrix4x4>();
					std::array<float, Matrix4x4::CellCount> vf = v.AsFloatArray();
					bool changed = false;

					if (ImGui::DragFloat4("", vf.data()))
					{
						changed = true;
					}

					if (ImGui::DragFloat4("", vf.data() + 4))
					{
						changed = true;
					}

					if (ImGui::DragFloat4("", vf.data() + 8))
					{
						changed = true;
					}

					if (ImGui::DragFloat4("", vf.data() + 12))
					{
						changed = true;
					}

					if (changed)
					{
						material.SetValue(name, Matrix4x4(vf));
					}
					break;
				}
				default:
					break;
				}

				ImGui::PopID();
				i++;
			}
		);

	}
}