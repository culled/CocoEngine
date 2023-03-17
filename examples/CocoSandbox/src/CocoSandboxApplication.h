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

#include "HelloTriangleRenderPass.h"

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

struct ShaderUniformObject
{
	float BaseColor[4];
	uint8_t Padding[128];

	ShaderUniformObject();
};

class CocoSandboxApplication final : public Coco::Application
{
private:
	static constexpr Coco::Array<const char*, 2> s_textureFiles = {
		"assets/textures/UV_Grid.png",
		"assets/textures/LargeBlocks.png"
	};

	Coco::Ref<Coco::MainLoopTickListener> _tickListener;
	Coco::Input::InputService* _inputService;
	Coco::Rendering::RenderingService* _renderService;
	Coco::Windowing::WindowingService* _windowService;
	Coco::Windowing::Window* _window = nullptr;
	Coco::Ref<Coco::Rendering::CameraComponent> _camera;
	Coco::Vector3 _cameraPosition;
	Coco::Vector3 _cameraEulerAngles;

	Coco::Ref<Coco::Rendering::Shader> _shader;
	ShaderUniformObject _shaderUO;
	Coco::Ref<Coco::Rendering::Material> _material;
	Coco::Ref<Coco::Rendering::Texture> _texture;
	uint _textureIndex = 0;

	Coco::Ref<Coco::Rendering::Mesh> _mesh;
	Coco::Matrix4x4 _meshTransform;

	Coco::Ref<HelloTriangleRenderPass> _rp;

public:
	CocoSandboxApplication(Coco::Engine* engine);
	~CocoSandboxApplication() final;

	void Start() final;
	Coco::Ref<Coco::Rendering::CameraComponent> GetCamera() const noexcept final { return _camera; }

	void Tick(double deltaTime);
};

