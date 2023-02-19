#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Events/Event.h>
#include <Coco/Core/Types/Optional.h>

#include "InputTypes.h"

namespace Coco::Input
{
	/// <summary>
	/// A state for a mouse
	/// </summary>
	struct MouseState
	{
		/// <summary>
		/// The maximum number of mouse buttons
		/// </summary>
		static const int ButtonCount = 6;

		/// <summary>
		/// Mouse button states
		/// </summary>
		bool ButtonStates[ButtonCount];

		/// <summary>
		/// The mouse position
		/// </summary>
		Vector2Int Position = Vector2Int::Zero;

		/// <summary>
		/// The mouse scroll delta
		/// </summary>
		Vector2Int ScrollDelta = Vector2Int::Zero;
	};

	/// <summary>
	/// Possible state changes for a mouse
	/// </summary>
	struct MouseStateChange
	{
		Optional<MouseButton> Button;
		bool IsButtonPressed = false;
		Optional<Vector2Int> Position;
		Vector2Int MoveDelta = Vector2Int::Zero;
		Optional<Vector2Int> ScrollDelta;
		
		static MouseStateChange MoveStateChange(const Vector2Int& newPosition, const Vector2Int& delta);
		static MouseStateChange ScrollStateChange(const Vector2Int& scrollDelta);
		static MouseStateChange ButtonStateChange(MouseButton button, bool isPressed);
	};

	class COCOAPI Mouse
	{
	public:
		Event<MouseButton> OnButtonPressed;
		Event<MouseButton> OnButtonReleased;
		Event<MouseButton> OnDoubleClicked;
		Event<const Vector2Int&, const Vector2Int&> OnMoved;
		Event<const Vector2Int&> OnScrolled;

	private:
		friend class InputService;

		List<MouseStateChange> _preProcessStateChanges;
		MouseState _preProcessState = {};
		MouseState _currentState = {};
		MouseState _previousState = {};

	public:
		/// <summary>
		/// Updates the pressed state for a mouse button
		/// </summary>
		/// <param name="button">The mouse button</param>
		/// <param name="isPressed">True if the mouse button is pressed</param>
		void UpdateButtonState(MouseButton button, bool isPressed);

		/// <summary>
		/// Updates the position state for the mouse
		/// </summary>
		/// <param name="newPosition">The new mouse position</param>
		void UpdatePositionState(const Vector2Int& newPosition);

		/// <summary>
		/// Updates the scroll wheel state for the mouse
		/// </summary>
		/// <param name="scrollDelta">The scroll delta</param>
		void UpdateScrollState(const Vector2Int& scrollDelta);

		/// <summary>
		/// Called when a mouse button has been double-clicked
		/// </summary>
		/// <param name="button">The mouse button</param>
		void DoubleClicked(MouseButton button);

		/// <summary>
		/// Gets if the given mouse button is currently pressed
		/// </summary>
		/// <param name="button">The mouse button</param>
		/// <returns>True if the mouse button is currently pressed</returns>
		bool IsButtonPressed(MouseButton button) const;

		/// <summary>
		/// Gets if the given mouse button was just pressed (unpressed -> pressed) within the last tick
		/// </summary>
		/// <param name="button">The mouse button</param>
		/// <returns>True if the mouse button was pressed since the last tick</returns>
		bool WasButtonJustPressed(MouseButton button) const;

		/// <summary>
		/// Gets if the given mouse button was just released (pressed -> unpressed) within the last tick
		/// </summary>
		/// <param name="button">The mouse button</param>
		/// <returns>True if the mouse button was released since the last tick</returns>
		bool WasButtonJustReleased(MouseButton button) const;

		/// <summary>
		/// Gets the current position of the mouse
		/// </summary>
		/// <returns>The mouse position</returns>
		Vector2Int GetPosition() const { return _currentState.Position; }

		/// <summary>
		/// Gets the amount the mouse has moved since the last tick
		/// </summary>
		/// <returns>The movement delta since last tick</returns>
		Vector2Int GetDelta() const { return _currentState.Position - _previousState.Position; }

		/// <summary>
		/// Gets the amount the scroll wheel has moved since last tick
		/// </summary>
		/// <returns>The scroll wheel delta</returns>
		Vector2Int GetScrollWheelDelta() const { return _currentState.ScrollDelta; }

	private:
		/// <summary>
		/// Processes all state changes since the last tick
		/// </summary>
		void ProcessCurrentState();

		/// <summary>
		/// Saves the current state as the previous state
		/// </summary>
		void SavePreviousState();
	};
}
