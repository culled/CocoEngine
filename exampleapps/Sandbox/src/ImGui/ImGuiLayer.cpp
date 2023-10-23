#include "ImGuiLayer.h"

#include <imgui.h>
#include <Coco/Core/MainLoop/MainLoop.h>

#include <Coco/Rendering/RenderService.h>
#include <Coco/Windowing/WindowService.h>

using namespace Coco;
using namespace Coco::Rendering;
using namespace Coco::Windowing;

ImGuiLayer::ImGuiLayer(RenderViewProvider3D& viewProvider3D, SceneDataProvider3D& sceneProvider3D) :
	_viewProvider3D(viewProvider3D),
	_sceneProvider3D(sceneProvider3D),
	_movingAverageCount(30),
	_averageFps(0),
	_averageFrameTime(0)
{}

void ImGuiLayer::Draw()
{
	ImGui::Begin("Settings");

	std::array<const char*, 4> msaaTexts = { "One", "Two", "Four", "Eight" };
	MSAASamples currentMSAASamples = _viewProvider3D.GetMSAASamples();

	if (ImGui::BeginCombo("MSAA samples", msaaTexts[static_cast<int>(currentMSAASamples)]))
	{
		for (int i = 0; i < 4; i++)
		{
			MSAASamples s = static_cast<MSAASamples>(i);
			if (ImGui::Selectable(msaaTexts[i], currentMSAASamples == s))
				_viewProvider3D.SetMSAASamples(s);

		}

		ImGui::EndCombo();
	}

	Ref<Window> mainWindow = WindowService::Get()->GetMainWindow();
	Ref<GraphicsPresenter> windowPresenter = mainWindow->GetPresenter();
	bool isVsync = windowPresenter->GetVSync() == VSyncMode::EveryVBlank;
	if (ImGui::Checkbox("VSync", &isVsync))
	{
		windowPresenter->SetVSync(isVsync ? VSyncMode::EveryVBlank : VSyncMode::Immediate);
	}

	bool drawBounds = _sceneProvider3D.GetDrawBounds();
	if (ImGui::Checkbox("Draw Bounds", &drawBounds))
	{
		_sceneProvider3D.SetDrawBounds(drawBounds);
	}

	ImGui::End();
}

void ImGuiLayer::DrawPostRender()
{
	//ImGui::ShowDemoWindow();

	ImGui::Begin("Info");

	const TickInfo& currentTick = MainLoop::cGet()->GetCurrentTick();

	_averageFps += static_cast<int>(Math::Round(((1.0 / currentTick.UnscaledDeltaTime) - _averageFps) / _movingAverageCount));
	_averageFrameTime += (currentTick.UnscaledDeltaTime - _averageFrameTime) / _movingAverageCount;

	ImGui::Text("Frame Time: %.3fms (%i fps)", _averageFrameTime * 1000.0, _averageFps);

	ImGui::Separator();

	if (ImGui::TreeNode("Rendering Info"))
	{
		Rendering::RenderService* rs = Rendering::RenderService::Get();
		const Rendering::RenderStats& renderStats = rs->GetRenderStats();
		ImGui::Text("%i vertices", renderStats.VertexCount);
		ImGui::Text("%i triangles", renderStats.TrianglesDrawn);
		ImGui::Text("%i draw call(s)", renderStats.DrawCalls);
		ImGui::Text("Total render time: %.3fms", renderStats.TotalExecutionTime.GetMilliseconds());
		ImGui::Text("Render wait time: %.3fms", renderStats.RenderSyncWait.GetMilliseconds());

		size_t renderIndex = 0;

		for (const auto& stats : rs->GetIndividualRenderStats())
		{
			if (ImGui::TreeNode(reinterpret_cast<void*>(renderIndex), "Render %i", renderIndex))
			{
				ImGui::Text("%i vertices", stats.VertexCount);
				ImGui::Text("%i triangles", stats.TrianglesDrawn);
				ImGui::Text("%i draw call(s)", stats.DrawCalls);
				ImGui::Text("Render time: %.3fms", stats.TotalExecutionTime.GetMilliseconds());

				if (ImGui::TreeNode("Pass Execution"))
				{
					for (const auto& kvp : stats.PassExecutionTime)
					{
						ImGui::Text("%s: %.3fms", kvp.first.c_str(), kvp.second.GetMilliseconds());
					}

					ImGui::TreePop();
				}

				ImGui::TreePop();
			}

			renderIndex++;
		}

		ImGui::TreePop();
	}

	ImGui::End();
}
