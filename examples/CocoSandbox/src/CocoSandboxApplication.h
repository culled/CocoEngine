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
#include <Coco/Core/Scene/Scene.h>

namespace Coco
{
	class Engine;
	class MainLoopTickListener;
}

namespace Coco::Windowing
{
	class WindowingService;
	class Window;
}

namespace Coco::Input
{
	class InputService;
}

namespace Coco::Rendering
{
	class RenderingService;
}

class CocoSandboxApplication final : public Coco::Application
{
private:
	static constexpr Coco::Array<const char*, 2> s_textureFiles = {
		"textures/UV_Grid.ctexture",
		"textures/LargeBlocks.ctexture"
	};

	Coco::Ref<Coco::MainLoopTickListener> _tickListener;
	Coco::Input::InputService* _inputService;
	Coco::Rendering::RenderingService* _renderService;
	Coco::Windowing::WindowingService* _windowService;
	Coco::WeakManagedRef<Coco::Windowing::Window> _window;

	Coco::Ref<Coco::Rendering::Shader> _shader;
	Coco::Ref<Coco::Rendering::Material> _material;
	Coco::Ref<Coco::Rendering::Texture> _texture;
	uint _textureIndex = 0;

	Coco::Ref<Coco::SceneEntity> _camera;
	Coco::Vector3 _cameraEulerAngles;

	Coco::Ref<Coco::SceneEntity> _obj;
	Coco::Ref<Coco::SceneEntity> _obj2;
	Coco::Ref<Coco::Rendering::Mesh> _mesh;

public:
	CocoSandboxApplication(Coco::Engine* engine);
	~CocoSandboxApplication() final;

	void Start() final;

	void Tick(double deltaTime);
};

