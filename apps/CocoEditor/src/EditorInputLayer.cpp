#include "EditorInputLayer.h"

#include "EditorApplication.h"
#include <Coco/Core/Engine.h>

using namespace Coco::Input;

namespace Coco
{
	const int EditorInputLayer::Priority = -20;

	bool EditorInputLayer::HandleKeyboardStateChange(const Input::KeyboardStateChange& state, const Input::Keyboard& keyboard)
	{
		if (!state.Key.has_value() || !state.IsPressed)
			return false;

		bool control = keyboard.IsKeyPressed(KeyboardKey::LeftControl) || keyboard.IsKeyPressed(KeyboardKey::RightControl);
		bool shift = keyboard.IsKeyPressed(KeyboardKey::LeftShift) || keyboard.IsKeyPressed(KeyboardKey::RightShift);
		EditorApplication* app = EditorApplication::Get();

		switch (state.Key.value())
		{
		case KeyboardKey::N:
		{
			if (control && !shift)
			{
				app->NewScene();
				return true;
			}
			break;
		}
		case KeyboardKey::O:
		{
			if (control && !shift)
			{
				app->OpenScene();
				return true;
			}
			break;
		}
		case KeyboardKey::S:
		{
			if (control && shift)
			{
				app->SaveSceneAs();
				return true;
			}
			break;
		}
		}

		return false;
	}
}