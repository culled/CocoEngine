#include "ImGuiCocoPlatform.h"

#include <Coco/Input/InputService.h>
#include <Coco/Windowing/WindowService.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Core/Engine.h>
#include <imgui.h>
#include "ImGuiRenderPass.h"

namespace Coco::ImGui
{
	ImGuiKey ToImGuiKey(Input::KeyboardKey key)
	{
		using namespace Coco::Input;

		switch (key)
		{
		case KeyboardKey::Backspace:
			return ImGuiKey_Backspace;
		case KeyboardKey::Tab:
			return ImGuiKey_Tab;

        case KeyboardKey::Enter:
            return ImGuiKey_Enter;

        case KeyboardKey::Pause:
            return ImGuiKey_Pause;
        case KeyboardKey::CapsLock:
            return ImGuiKey_CapsLock;

        case KeyboardKey::Escape:
            return ImGuiKey_Escape;

        case KeyboardKey::Space:
            return ImGuiKey_Space;
        case KeyboardKey::PageUp:
            return ImGuiKey_PageUp;
        case KeyboardKey::PageDown:
            return ImGuiKey_PageDown;
        case KeyboardKey::End:
            return ImGuiKey_End;
        case KeyboardKey::Home:
            return ImGuiKey_Home;
        case KeyboardKey::ArrowLeft:
            return ImGuiKey_LeftArrow;
        case KeyboardKey::ArrowUp:
            return ImGuiKey_UpArrow;
        case KeyboardKey::ArrowRight:
            return ImGuiKey_RightArrow;
        case KeyboardKey::ArrowDown:
            return ImGuiKey_DownArrow;

        case KeyboardKey::PrintScreen:
            return ImGuiKey_PrintScreen;
        case KeyboardKey::Insert:
            return ImGuiKey_Insert;
        case KeyboardKey::Delete:
            return ImGuiKey_Delete;

        case KeyboardKey::D0:
            return ImGuiKey_0;
        case KeyboardKey::D1:
            return ImGuiKey_1;
        case KeyboardKey::D2:
            return ImGuiKey_2;
        case KeyboardKey::D3:
            return ImGuiKey_3;
        case KeyboardKey::D4:
            return ImGuiKey_4;
        case KeyboardKey::D5:
            return ImGuiKey_5;
        case KeyboardKey::D6:
            return ImGuiKey_6;
        case KeyboardKey::D7:
            return ImGuiKey_7;
        case KeyboardKey::D8:
            return ImGuiKey_8;
        case KeyboardKey::D9:
            return ImGuiKey_9;

        case KeyboardKey::A:
            return ImGuiKey_A;
        case KeyboardKey::B:
            return ImGuiKey_B;
        case KeyboardKey::C:
            return ImGuiKey_C;
        case KeyboardKey::D:
            return ImGuiKey_D;
        case KeyboardKey::E:
            return ImGuiKey_E;
        case KeyboardKey::F:
            return ImGuiKey_F;
        case KeyboardKey::G:
            return ImGuiKey_G;
        case KeyboardKey::H:
            return ImGuiKey_H;
        case KeyboardKey::I:
            return ImGuiKey_I;
        case KeyboardKey::J:
            return ImGuiKey_J;
        case KeyboardKey::K:
            return ImGuiKey_K;
        case KeyboardKey::L:
            return ImGuiKey_L;
        case KeyboardKey::M:
            return ImGuiKey_M;
        case KeyboardKey::N:
            return ImGuiKey_N;
        case KeyboardKey::O:
            return ImGuiKey_O;
        case KeyboardKey::P:
            return ImGuiKey_P;
        case KeyboardKey::Q:
            return ImGuiKey_Q;
        case KeyboardKey::R:
            return ImGuiKey_R;
        case KeyboardKey::S:
            return ImGuiKey_S;
        case KeyboardKey::T:
            return ImGuiKey_T;
        case KeyboardKey::U:
            return ImGuiKey_U;
        case KeyboardKey::V:
            return ImGuiKey_V;
        case KeyboardKey::W:
            return ImGuiKey_W;
        case KeyboardKey::X:
            return ImGuiKey_X;
        case KeyboardKey::Y:
            return ImGuiKey_Y;
        case KeyboardKey::Z:
            return ImGuiKey_Z;

        case KeyboardKey::LeftMeta:
            return ImGuiKey_LeftSuper;
        case KeyboardKey::RightMeta:
            return ImGuiKey_RightSuper;

        case KeyboardKey::Numpad0:
            return ImGuiKey_Keypad0;
        case KeyboardKey::Numpad1:
            return ImGuiKey_Keypad1;
        case KeyboardKey::Numpad2:
            return ImGuiKey_Keypad2;
        case KeyboardKey::Numpad3:
            return ImGuiKey_Keypad3;
        case KeyboardKey::Numpad4:
            return ImGuiKey_Keypad4;
        case KeyboardKey::Numpad5:
            return ImGuiKey_Keypad5;
        case KeyboardKey::Numpad6:
            return ImGuiKey_Keypad6;
        case KeyboardKey::Numpad7:
            return ImGuiKey_Keypad7;
        case KeyboardKey::Numpad8:
            return ImGuiKey_Keypad8;
        case KeyboardKey::Numpad9:
            return ImGuiKey_Keypad9;

        case KeyboardKey::NumpadMultiply:
            return ImGuiKey_KeypadMultiply;
        case KeyboardKey::NumpadPlus:
            return ImGuiKey_KeypadAdd;
        case KeyboardKey::NumpadMinus:
            return ImGuiKey_KeypadSubtract;
        case KeyboardKey::NumpadDecimal:
            return ImGuiKey_KeypadDecimal;
        case KeyboardKey::NumpadDivide:
            return ImGuiKey_KeypadDivide;

        case KeyboardKey::F1:
            return ImGuiKey_F1;
        case KeyboardKey::F2:
            return ImGuiKey_F2;
        case KeyboardKey::F3:
            return ImGuiKey_F3;
        case KeyboardKey::F4:
            return ImGuiKey_F4;
        case KeyboardKey::F5:
            return ImGuiKey_F5;
        case KeyboardKey::F6:
            return ImGuiKey_F6;
        case KeyboardKey::F7:
            return ImGuiKey_F7;
        case KeyboardKey::F8:
            return ImGuiKey_F8;
        case KeyboardKey::F9:
            return ImGuiKey_F9;
        case KeyboardKey::F10:
            return ImGuiKey_F10;
        case KeyboardKey::F11:
            return ImGuiKey_F11;
        case KeyboardKey::F12:
            return ImGuiKey_F12;

        case KeyboardKey::NumLock:
            return ImGuiKey_NumLock;
        case KeyboardKey::ScrollLock:
            return ImGuiKey_ScrollLock;

        case KeyboardKey::LeftShift:
            return ImGuiKey_LeftShift;
        case KeyboardKey::RightShift:
            return ImGuiKey_RightShift;
        case KeyboardKey::LeftControl:
            return ImGuiKey_LeftCtrl;
        case KeyboardKey::RightControl:
            return ImGuiKey_RightCtrl;
        case KeyboardKey::LeftAlt:
            return ImGuiKey_LeftAlt;
        case KeyboardKey::RightAlt:
            return ImGuiKey_RightAlt;

        case KeyboardKey::Semicolon:
            return ImGuiKey_Semicolon;

        case KeyboardKey::Equals:
            return ImGuiKey_Equal;
        case KeyboardKey::Comma:
            return ImGuiKey_Comma;
        case KeyboardKey::Minus:
            return ImGuiKey_Minus;
        case KeyboardKey::Period:
            return ImGuiKey_Period;

        case KeyboardKey::ForwardSlash:
            return ImGuiKey_Slash;

        case KeyboardKey::BackQuote:
            return ImGuiKey_GraveAccent;

        case KeyboardKey::LeftBracket:
            return ImGuiKey_LeftBracket;

        case KeyboardKey::BackSlash:
            return ImGuiKey_Backslash;

        case KeyboardKey::RightBracket:
            return ImGuiKey_RightBracket;

        case KeyboardKey::SingleQuote:
            return ImGuiKey_Apostrophe;

        default:
            return ImGuiKey_None;
		}
	}

    ImGuiCocoPlatform::ImGuiCocoPlatform()
    {
        ImGuiIO& io = ::ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableSetMousePos; // TODO: set mouse cursor position
        io.BackendFlags = ImGuiBackendFlags_None;
        io.BackendPlatformName = "Coco";
        io.BackendRendererName = "Coco";

        // Setup ImGui shader
        GraphicsPipelineState pipelineState{};
        pipelineState.WindingMode = TriangleWindingMode::CounterClockwise;
        pipelineState.CullingMode = CullMode::None;
        pipelineState.DepthTestingMode = DepthTestingMode::Never;
        pipelineState.EnableDepthWrite = false;

        BlendState blendState{};
        blendState.ColorSourceFactor = BlendFactorMode::SourceAlpha;
        blendState.ColorDestinationFactor = BlendFactorMode::OneMinusSourceAlpha;
        blendState.ColorBlendOperation = BlendOperation::Add;

        blendState.AlphaSourceBlendFactor = BlendFactorMode::One;
        blendState.AlphaDestinationBlendFactor = BlendFactorMode::OneMinusSourceAlpha;
        blendState.AlphaBlendOperation = BlendOperation::Add;

        ResourceLibrary* resources = Engine::Get()->GetResourceLibrary();
        _shader = resources->Create<Shader>("ImGui", ImGuiRenderPass::sPassName);
        _shader->AddRenderPassShader(RenderPassShader(
            0,
            ImGuiRenderPass::sPassName,
            {
                ShaderStage("main", ShaderStageType::Vertex, "shaders/built-in/ImGui.vert.spv"),
                ShaderStage("main", ShaderStageType::Fragment, "shaders/built-in/ImGui.frag.spv")
            },
            pipelineState,
            {
                blendState
            },
            {
                ShaderVertexAttribute("Position", BufferDataType::Float3),
                ShaderVertexAttribute("Color", BufferDataType::Float4),
                ShaderVertexAttribute("UV", BufferDataType::Float2)
            },
            {
                ShaderDataUniform("projection", UniformScope::Global, ShaderStageFlags::Vertex, BufferDataType::Mat4x4)
            },
            {
                ShaderTextureUniform("sTexture", UniformScope::Instance, ShaderStageFlags::Fragment)
            }
            ));

        // Setup ImGui mesh
        _mesh = resources->Create<Mesh>("ImGui", true);

        // Setup the font texture
        int width, height;
        unsigned char* pixels = nullptr;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        _texture = resources->Create<Texture>(
            "ImGui",
            ImageDescription(
                static_cast<uint32>(width), static_cast<uint32>(height), 1,
                ImagePixelFormat::RGBA8, ImageColorSpace::sRGB,
                ImageUsageFlags::Sampled | ImageUsageFlags::TransferDestination
            ),
            ImageSamplerDescription());

        _texture->SetPixels(0, pixels, static_cast<uint64>(width) * height * GetPixelFormatChannelCount(ImagePixelFormat::RGBA8));

        io.Fonts->SetTexID(static_cast<void*>(_texture.Get()));

        // Setup ImGui material
        _material = resources->Create<Material>("ImGui", _shader);
        _material->SetTexture("sTexture", _texture);
    }

    ImGuiCocoPlatform::~ImGuiCocoPlatform()
    {
        _shader.Invalidate();
        _mesh.Invalidate();
        _material.Invalidate();
    }

    UniqueRef<Rendering::RenderView> ImGuiCocoPlatform::CreateRenderView(
        const Rendering::CompiledRenderPipeline& pipeline,
        const SizeInt& backbufferSize,
        std::span<Ref<Rendering::Image>> backbuffers)
    {
        using namespace Coco::Rendering;

        std::vector<RenderTarget> rts;

        for (size_t i = 0; i < backbuffers.size(); i++)
            rts.emplace_back(backbuffers[i], Vector4::Zero);

        ImDrawData* drawData = ::ImGui::GetDrawData();

        RectInt viewport(
            Vector2Int(static_cast<int>(drawData->DisplayPos.x), static_cast<int>(drawData->DisplayPos.y)),
            SizeInt(static_cast<uint32>(drawData->DisplaySize.x), static_cast<uint32>(drawData->DisplaySize.y))
        );

        Matrix4x4 projection = RenderService::Get()->GetPlatform()->CreateOrthographicProjection(
            viewport.GetLeft(),
            viewport.GetRight(),
            viewport.GetTop(),
            viewport.GetBottom(),
            -1.0, 1.0);

        return CreateUniqueRef<RenderView>(viewport, viewport, Matrix4x4::Identity, projection, rts);
    }

    void ImGuiCocoPlatform::GatherSceneData(RenderView& renderView)
    {
        using namespace Coco::Rendering;

        ImDrawData* drawData = ::ImGui::GetDrawData();

        VertexDataFormat format{};
        format.HasColor = true;
        format.HasUV0 = true;

        if (drawData->TotalVtxCount > 0)
        {
            renderView.AddMaterial(*_material);

            _mesh->ClearSubmeshes();

            VertexDataFormat format{};
            format.HasUV0 = true;
            format.HasColor = true;

            std::vector<VertexData> vertices(drawData->TotalVtxCount);
            std::vector<uint32> indices(drawData->TotalIdxCount);
            uint64 vertexOffset = 0;
            uint64 indexOffset = 0;

            for (int n = 0; n < drawData->CmdListsCount; n++)
            {
                const ImDrawList* drawList = drawData->CmdLists[n];

                for (int v = 0; v < drawList->VtxBuffer.Size; v++)
                {
                    ImDrawVert* imVert = drawList->VtxBuffer.Data + v;
                    VertexData& vert = vertices.at(v + vertexOffset);

                    vert.Position = Vector3(imVert->pos.x, imVert->pos.y, 0.0);

                    ImVec4 imGuiColor = ::ImGui::ColorConvertU32ToFloat4(imVert->col);
                    Color c(imGuiColor.x, imGuiColor.y, imGuiColor.z, imGuiColor.w);
                    c.ConvertToLinear();
                    vert.Color = Vector4(c.R, c.G, c.B, c.A);

                    vert.UV0 = Vector2(imVert->uv.x, imVert->uv.y);
                }

                for (int i = 0; i < drawList->IdxBuffer.Size; i++)
                {
                    indices.at(i + indexOffset) = *(drawList->IdxBuffer.Data + i) + static_cast<uint32>(vertexOffset);
                }

                vertexOffset += drawList->VtxBuffer.Size;
                indexOffset += drawList->IdxBuffer.Size;
            }

            _mesh->SetVertices(format, vertices);
            _mesh->SetIndices(indices, 0);
            _mesh->Apply();

            renderView.AddMesh(*_mesh);
        }
    }

    bool ImGuiCocoPlatform::NewFrame(const TickInfo& tickInfo)
	{
		using namespace Coco::Windowing;
		using namespace Coco::Input;

		WindowService* windowing = WindowService::Get();
		if (!windowing)
			throw std::exception("No active WindowService found");

		InputService* input = InputService::Get();
		if (!input)
			throw std::exception("No active InputService found");

		Ref<Window> mainWindow = windowing->GetMainWindow();
		if (!mainWindow.IsValid())
			return false;

		ImGuiIO& io = ::ImGui::GetIO();

		// Add basic info
		io.DeltaTime = static_cast<float>(tickInfo.UnscaledDeltaTime);

		// Add window-specific info
		SizeInt displaySize = mainWindow->GetClientAreaSize();
		io.DisplaySize = ImVec2(static_cast<float>(displaySize.Width), static_cast<float>(displaySize.Height));

		// Add mouse events
		Mouse* mouse = input->GetMouse();
		if (!io.WantSetMousePos)
		{
			Vector2Int mousePos = mouse->GetPosition();
			io.AddMousePosEvent(static_cast<float>(mousePos.X), static_cast<float>(mousePos.Y));
		}

		io.AddMouseButtonEvent(0, mouse->IsButtonPressed(MouseButton::Left));
		io.AddMouseButtonEvent(1, mouse->IsButtonPressed(MouseButton::Right));
		io.AddMouseButtonEvent(2, mouse->IsButtonPressed(MouseButton::Middle));
		io.AddMouseButtonEvent(3, mouse->IsButtonPressed(MouseButton::Button3));
		io.AddMouseButtonEvent(4, mouse->IsButtonPressed(MouseButton::Button4));

		Vector2Int scrollDelta = mouse->GetScrollWheelDelta();
		if (scrollDelta != Vector2Int::Zero)
			io.AddMouseWheelEvent(static_cast<float>(scrollDelta.X), static_cast<float>(scrollDelta.Y));

		Keyboard* keyboard = input->GetKeyboard();
        io.AddKeyEvent(ImGuiMod_Ctrl, keyboard->IsKeyPressed(KeyboardKey::LeftControl) || keyboard->IsKeyPressed(KeyboardKey::RightControl));
        io.AddKeyEvent(ImGuiMod_Shift, keyboard->IsKeyPressed(KeyboardKey::LeftShift) || keyboard->IsKeyPressed(KeyboardKey::RightShift));
        io.AddKeyEvent(ImGuiMod_Alt, keyboard->IsKeyPressed(KeyboardKey::LeftAlt) || keyboard->IsKeyPressed(KeyboardKey::RightAlt));
        io.AddKeyEvent(ImGuiMod_Super, keyboard->IsKeyPressed(KeyboardKey::LeftMeta) || keyboard->IsKeyPressed(KeyboardKey::RightMeta));

		for (int i = 0; i < KeyboardState::KeyCount; i++)
		{
			KeyboardKey key = static_cast<KeyboardKey>(i);
            ImGuiKey imGuiKey = ToImGuiKey(key);

            if (imGuiKey == ImGuiKey_None)
                continue;

            if (keyboard->WasKeyJustPressed(key))
                io.AddKeyEvent(imGuiKey, true);
            else if (keyboard->WasKeyJustReleased(key))
                io.AddKeyEvent(imGuiKey, false);
		}
        
        return true;
	}
}