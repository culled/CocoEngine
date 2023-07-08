#pragma once

#include <Coco/Core/Application.h>
#include <Coco/Core/Events/Query.h>
#include <Coco/Core/Events/Event.h>

#include <Coco/Core/Types/Quaternion.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Array.h>
#include <Coco/Rendering/Material.h>
#include <Coco/Rendering/Texture.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/ECS/Entity.h>

namespace Coco
{
	class Engine;
	class MainLoopTickListener;
}

namespace Coco::Input
{
	class InputService;
}

namespace Coco::Windowing
{
	class WindowingService;
	class Window;
}

namespace Coco::Rendering
{
	class RenderingService;
}

namespace Coco::ECS
{
	class ECSService;
}

class CocoSandboxApplication final : public Coco::Application
{
private:
	static constexpr Coco::Array<const char*, 2> s_textureFiles = {
		"textures/UV_Grid.ctexture",
		"textures/LargeBlocks.ctexture"
	};

	Coco::Ref<Coco::MainLoopTickListener> _tickListener;
	Coco::Ref<Coco::MainLoopTickListener> _renderTickListener;

	Coco::Input::InputService* _inputService;

	Coco::Windowing::WindowingService* _windowService;
	Coco::Ref<Coco::Windowing::Window> _window;

	Coco::Rendering::RenderingService* _renderService;
	Coco::Ref<Coco::Rendering::Shader> _shader;
	Coco::Ref<Coco::Rendering::Material> _material;
	Coco::Ref<Coco::Rendering::Texture> _texture;
	uint _textureIndex = 0;

	Coco::Ref<Coco::Rendering::Mesh> _mesh;

	Coco::ECS::ECSService* _ecsService;
	Coco::ECS::EntityID _cameraEntityID;
	Coco::ECS::EntityID _obj2ID;

public:
	CocoSandboxApplication();
	~CocoSandboxApplication() final;

	void Start() final;

	void Tick(double deltaTime);
	void RenderTick(double deltaTime);
};

