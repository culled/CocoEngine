#pragma once

#include <Coco/Rendering/Providers/SceneDataProvider.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Shader.h>
#include <Coco/Rendering/Material.h>

using namespace Coco::Rendering;

namespace Coco
{
	class RenderTest :
		public SceneDataProvider
	{
	private:
		Ref<Mesh> _mesh;
		Ref<Shader> _shader;
		Ref<Material> _material;

	public:
		RenderTest();

		// Inherited via SceneDataProvider
		void GatherSceneData(RenderView& renderView) override;
	};
}