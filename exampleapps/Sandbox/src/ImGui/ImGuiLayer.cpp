#include "ImGuiLayer.h"

#include <imgui.h>
#include <Coco/Core/MainLoop/MainLoop.h>

using namespace Coco;

void ImGuiLayer::Draw()
{
	ImGui::Begin("Info");

	const TickInfo& currentTick = MainLoop::cGet()->GetCurrentTick();
	
	ImGui::Text("Frame Time: %.3fms (%i fps)", currentTick.DeltaTime * 1000.0, static_cast<int>(1.0 / currentTick.DeltaTime));

	ImGui::End();
}
