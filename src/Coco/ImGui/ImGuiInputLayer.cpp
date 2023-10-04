#include "ImGuiInputLayer.h"

#include <imgui.h>

namespace Coco::ImGuiCoco
{
	const int ImGuiInputLayer::Priority = -10;

	bool ImGuiInputLayer::HandleMouseStateChange(const Input::MouseStateChange& state)
	{
		ImGuiIO& io = ImGui::GetIO();

		return io.WantCaptureMouse;
	}

	bool ImGuiInputLayer::HandleKeyboardStateChange(const Input::KeyboardStateChange& state)
	{
		ImGuiIO& io = ImGui::GetIO();

		return io.WantCaptureKeyboard;
	}
}