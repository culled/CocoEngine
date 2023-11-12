#include "MeshPrimitiveResourceDialog.h"

#include "../UI/UIUtils.h"
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/MeshUtilities.h>

using namespace Coco::Rendering;

namespace Coco
{
	MeshPrimitiveResourceDialog::MeshPrimitiveResourceDialog() :
		_newResourceName(),
		_newResourceType(0),
		_savePath(),
		_newResourceSize(Vector3::One),
		_newResourceFlip(false),
		_newResourceSphereOptions(16, 8)
	{}

	void MeshPrimitiveResourceDialog::SetSavePath(const FilePath& savePath)
	{
		_savePath = savePath;
	}

	void MeshPrimitiveResourceDialog::Opened()
	{
		_newResourceName = "New Mesh";
		_newResourceType = 0;
	}

	void MeshPrimitiveResourceDialog::Draw()
	{
		UIUtils::DrawInputStringEdit("Resource Name", _newResourceName, 64);

		if (ImGui::BeginCombo("Primitive Type", GetPrimitiveNameFromType(_newResourceType)))
		{
			for (int i = 0; i < 5; i++)
			{
				bool selected = i == _newResourceType;

				if (ImGui::Selectable(GetPrimitiveNameFromType(i), &selected))
				{
					_newResourceType = i;
				}
			}

			ImGui::EndCombo();
		}

		switch (_newResourceType)
		{
		case 0:
		case 1:
		case 2:
			DrawQuadOptions();
			break;
		case 3:
			DrawCubeOptions();
			break;
		case 4:
			DrawSphereOptions();
			break;
		default:
			break;
		}

		if (ImGui::Button("Create") && !_newResourceName.empty())
		{
			auto& resources = Engine::Get()->GetResourceLibrary();
			SharedRef<Mesh> mesh = resources.Create<Mesh>(_newResourceName);

			SetMeshData(mesh);

			string fileName = _newResourceName + ".cmesh";
			resources.Save(FilePath::CombineToPath(_savePath, fileName), mesh, false);

			mesh->Apply();

			Close();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
			Close();
	}

	const char* MeshPrimitiveResourceDialog::GetPrimitiveNameFromType(int type)
	{
		switch (type)
		{
		case 0:
			return "XY Quad";
		case 1:
			return "XZ Quad";
		case 2:
			return "ZY Quad";
		case 3:
			return "Cube";
		case 4:
			return "UV Sphere";
		default:
			return "Unknown";
		}
	}

	void MeshPrimitiveResourceDialog::DrawQuadOptions()
	{
		auto size = _newResourceSize.AsFloatArray();
		if (ImGui::DragFloat2("Size", size.data(), 0.1f))
			_newResourceSize = Vector3(size);

		ImGui::Checkbox("Flip Normals", &_newResourceFlip);
	}

	void MeshPrimitiveResourceDialog::DrawCubeOptions()
	{
		auto size = _newResourceSize.AsFloatArray();
		if (ImGui::DragFloat3("Size", size.data(), 0.1f))
			_newResourceSize = Vector3(size);

		ImGui::Checkbox("Flip Normals", &_newResourceFlip);
	}

	void MeshPrimitiveResourceDialog::DrawSphereOptions()
	{
		float radius = static_cast<float>(_newResourceSize.X);
		if (ImGui::DragFloat("Radius", &radius, 0.1f))
			_newResourceSize.X = radius;

		ImGui::DragInt("Vertical Slices", &_newResourceSphereOptions.X, 1.f, 1);
		ImGui::DragInt("Horizontal Slices", &_newResourceSphereOptions.Y, 1.f, 1);

		ImGui::Checkbox("Flip Normals", &_newResourceFlip);
	}

	void MeshPrimitiveResourceDialog::SetMeshData(SharedRef<Rendering::Mesh> mesh)
	{
		std::vector<VertexData> vertices;
		std::vector<uint32> indices;

		switch (_newResourceType)
		{
		case 0:
			MeshUtilities::CreateXYGrid(_newResourceSize.XY(), Vector3::Zero, vertices, indices, 0, _newResourceFlip);
			break;
		case 1:
			MeshUtilities::CreateXZGrid(_newResourceSize.XY(), Vector3::Zero, vertices, indices, 0, _newResourceFlip);
			break;
		case 2:
			MeshUtilities::CreateZYGrid(_newResourceSize.XY(), Vector3::Zero, vertices, indices, 0, _newResourceFlip);
			break;
		case 3:
			MeshUtilities::CreateBox(_newResourceSize, Vector3::Zero, vertices, indices, 0, _newResourceFlip);
			break;
		case 4:
			MeshUtilities::CreateUVSphere(static_cast<uint32>(_newResourceSphereOptions.X), static_cast<uint32>(_newResourceSphereOptions.Y), _newResourceSize.X, Vector3::Zero, vertices, indices, _newResourceFlip);
			break;
		default:
			break;
		}

		mesh->SetVertices(VertexDataFormat(VertexAttrFlags::UV0), vertices);
		mesh->SetIndices(indices, 0);
	}
}