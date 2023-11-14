#include "SceneDataProvider2D.h"

#include <Coco/Rendering/MeshUtilities.h>
#include <Coco/Core/Engine.h>
#include <Coco/Rendering/RenderService.h>

SceneDataProvider2D::SceneDataProvider2D() :
	_transform(Vector2(50, 50), 0.0, Vector2::One)
{
	std::vector<VertexData> vertices;
	std::vector<uint32> indices;
	MeshUtilities::CreateXYGrid(Vector2::One * 50.0, Vector3::Zero, vertices, indices);

	ResourceLibrary& resourceLibrary = Engine::Get()->GetResourceLibrary();

	_mesh = resourceLibrary.Create<Mesh>("Mesh");
	VertexDataFormat format(VertexAttrFlags::UV0);
	_mesh->SetVertices(format, vertices);
	_mesh->SetIndices(indices, 0);

	ImageSamplerDescription sampler = ImageSamplerDescription::LinearClamp;
	_texture = resourceLibrary.Create<Texture>("TextureUI", "textures/UI Test 2.png", ImageColorSpace::sRGB, ImageUsageFlags::Sampled, sampler);

	_material = resourceLibrary.Create<Material>("Material2D");
	_material->SetValue("TintColor", Color::White);
	_material->SetValue("ColorTexture", _texture);
}

SceneDataProvider2D::~SceneDataProvider2D()
{
	_mesh.reset();
	_material.reset();
	_texture.reset();
}

void SceneDataProvider2D::GatherSceneData(RenderView& renderView)
{
	if (!_mesh->Apply())
		return;

	_transform.Rotate(MainLoop::cGet()->GetCurrentTick().DeltaTime, TransformSpace::Global);
	_transform.Recalculate();

	renderView.AddRenderObject(0, *_mesh, 0, _transform.GlobalTransform, 1, _material.get());
}
