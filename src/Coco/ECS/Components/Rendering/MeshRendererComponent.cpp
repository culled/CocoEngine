#include "ECSpch.h"
#include "MeshRendererComponent.h"

namespace Coco::ECS
{
	MeshRendererComponent::MeshRendererComponent() :
		MeshRendererComponent(nullptr, std::unordered_map<uint32, SharedRef<MaterialDataProvider>>())
	{}

	MeshRendererComponent::MeshRendererComponent(SharedRef<Rendering::Mesh> mesh, const std::unordered_map<uint32, SharedRef<MaterialDataProvider>>& materials) :
		Mesh(mesh),
		Materials(materials)
	{}
}