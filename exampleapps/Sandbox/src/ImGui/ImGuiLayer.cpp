#include "ImGuiLayer.h"

#include <imgui.h>
#include <Coco/Core/MainLoop/MainLoop.h>

#include <Coco/Rendering/RenderService.h>

using namespace Coco;

void ImGuiLayer::Draw()
{
}

void ImGuiLayer::DrawPostRender()
{
	//ImGui::ShowDemoWindow();

	ImGui::Begin("Info");

	const TickInfo& currentTick = MainLoop::cGet()->GetCurrentTick();

	ImGui::Text("Frame Time: %.3fms (%i fps)", currentTick.DeltaTime * 1000.0, static_cast<int>(1.0 / currentTick.DeltaTime));

	ImGui::Separator();

	if (ImGui::TreeNode("Rendering Info"))
	{
		Rendering::RenderService* rs = Rendering::RenderService::Get();
		const Rendering::RenderStats& renderStats = rs->GetRenderStats();
		ImGui::Text("%i vertices", renderStats.VertexCount);
		ImGui::Text("%i triangles", renderStats.TrianglesDrawn);
		ImGui::Text("%i draw call(s)", renderStats.DrawCalls);
		ImGui::Text("Total render time: %.3fms", renderStats.TotalExecutionTime.GetMilliseconds());

		int renderIndex = 0;

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
