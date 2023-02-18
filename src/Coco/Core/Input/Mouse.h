#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Events/Event.h>

namespace Coco::Input
{
	struct MouseState
	{
		static const int ButtonCount = 6;

		bool ButtonStates[ButtonCount];
		Vector2Int Position = Vector2Int::Zero;
	};

	class COCOAPI Mouse
	{
	public:
		/// <summary>
		/// Mouse buttons
		/// </summary>
		enum class Button
		{
			Left,
			Middle,
			Right,
			Button3,
			Button4,
			Unknown,
		};

		Event<Button> OnButtonPressed;
		Event<Button> OnButtonReleased;
		Event<Button> OnDoubleClicked;
		Event<const Vector2Int&, const Vector2Int&> OnMoved;
		Event<const Vector2Int&> OnScrolled;

	private:
		friend class InputService;

		MouseState _currentState = {};
		MouseState _previousState = {};

	public:
		void UpdateButtonState(Button button, bool isPressed);
		void UpdatePositionState(const Vector2Int& newPosition);
		void UpdateScrollState(const Vector2Int& scrollDelta);
		void DoubleClicked(Button button);

		bool IsButtonPressed(Button button) const;
		bool WasButtonJustPressed(Button button) const;
		bool WasButtonJustReleased(Button button) const;

		Vector2Int GetPosition() const { return _currentState.Position; }
		Vector2Int GetDelta() const { return _currentState.Position - _previousState.Position; }

	private:
		void SavePreviousState();
	};
}
