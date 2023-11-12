#include "ResourceDialog.h"

#include <Coco/Core/MainLoop/MainLoop.h>
#include <imgui.h>

namespace Coco
{
	ResourceDialog::ResourceDialog() :
		_isOpen(false),
		_tickListener(CreateManagedRef<TickListener>(this, &ResourceDialog::HandleTick, -5))
	{}

	ResourceDialog::~ResourceDialog()
	{
		_tickListener.Invalidate();
	}

	void ResourceDialog::Open()
	{
		if (_isOpen)
			return;

		_isOpen = true;
		MainLoop::Get()->AddListener(_tickListener);

		Opened();
	}

	void ResourceDialog::Close()
	{
		if (!_isOpen)
			return;

		_isOpen = false;
		MainLoop::Get()->RemoveListener(_tickListener);

		Closed();
	}

	void ResourceDialog::HandleTick(const TickInfo& tick)
	{
		if (!_isOpen)
			return;

		ImGui::OpenPopup(GetPopupName());

		if (ImGui::BeginPopupModal(GetPopupName(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			Draw();

			if (!_isOpen)
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		if (_isOpen && !ImGui::IsPopupOpen(GetPopupName()))
			Close();
	}
}