#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Array.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Optional.h>
#include <Coco/Core/Events/Event.h>

#include "InputTypes.h"

namespace Coco::Input
{
	/// @brief A state for a mouse
	struct MouseState
	{
		/// @brief The maximum number of mouse buttons
		static constexpr int ButtonCount = 6;

		/// @brief Mouse button states
		Array<bool, ButtonCount> ButtonStates = { false };

		/// @brief The mouse position
		Vector2Int Position = Vector2Int::Zero;

		/// @brief The mouse scroll delta
		Vector2Int ScrollDelta = Vector2Int::Zero;
	};

	/// @brief Possible state changes for a mouse
	struct MouseStateChange
	{
		/// @brief The button (if any) whose state changed
		Optional<MouseButton> Button;

		/// @brief If true, the button for this state was pressed, else it was released
		bool IsButtonPressed = false;

		/// @brief The new mouse position (if any)
		Optional<Vector2Int> Position;

		/// @brief If the position has changed, this is the delta movement since the last state change
		Vector2Int MoveDelta = Vector2Int::Zero;

		/// @brief The mouse scroll delta (if any)
		Optional<Vector2Int> ScrollDelta;
		
		/// @brief Creates a mouse state change for moving the mouse
		/// @param newPosition The new mouse position
		/// @param delta The delta between the new mouse position and the last position
		/// @return A mouse state change
		static MouseStateChange MoveStateChange(const Vector2Int& newPosition, const Vector2Int& delta) noexcept;

		/// @brief Creates a mouse state change for scrolling the mouse
		/// @param scrollDelta The amount the mouse was scrolled
		/// @return A mouse state change
		static MouseStateChange ScrollStateChange(const Vector2Int& scrollDelta) noexcept;

		/// @brief Creates a mouse state change for pressing or releasing a mouse button
		/// @param button The mouse button
		/// @param isPressed True if the button was pressed, false if it was released
		/// @return A mouse state change
		static MouseStateChange ButtonStateChange(MouseButton button, bool isPressed) noexcept;
	};

	/// @brief A mouse input device
	class COCOAPI Mouse
	{
	public:
		/// @brief Invoked when a mouse button is pressed
		Event<MouseButton> OnButtonPressed;

		/// @brief Invoked when a mouse button is released
		Event<MouseButton> OnButtonReleased;

		/// @brief Invoked when a mouse button is double-clicked
		Event<MouseButton> OnDoubleClicked;

		/// @brief Invoked when the mouse is moved
		Event<const Vector2Int&, const Vector2Int&> OnMoved;

		/// @brief Invoked when the mouse is scrolled
		Event<const Vector2Int&> OnScrolled;

	private:
		friend class InputService;

		List<MouseStateChange> _preProcessStateChanges;
		MouseState _preProcessState = {};
		MouseState _currentState = {};
		MouseState _previousState = {};

		bool _isFirstState = true;

	public:
		/// @brief Updates the pressed state for a mouse button
		/// @param button The mouse button
		/// @param isPressed True if the mouse button is pressed
		void UpdateButtonState(MouseButton button, bool isPressed);

		/// @brief Updates the position state for the mouse
		/// @param newPosition The new mouse position
		void UpdatePositionState(const Vector2Int& newPosition);

		/// @brief Updates the scroll wheel state for the mouse
		/// @param scrollDelta The scroll delta
		void UpdateScrollState(const Vector2Int& scrollDelta);

		/// @brief Called when a mouse button has been double-clicked
		/// @param button The mouse button
		void DoubleClicked(MouseButton button);

		/// @brief Gets if the given mouse button is currently pressed
		/// @param button The mouse button
		/// @return True if the mouse button is currently pressed
		bool IsButtonPressed(MouseButton button) const noexcept;

		/// @brief Gets if the given mouse button was just pressed (unpressed -> pressed) within the last tick
		/// @param button The mouse button
		/// @return True if the mouse button was pressed since the last tick
		bool WasButtonJustPressed(MouseButton button) const noexcept;

		/// @brief Gets if the given mouse button was just released (pressed -> unpressed) within the last tick
		/// @param button The mouse button
		/// @return True if the mouse button was released since the last tick
		bool WasButtonJustReleased(MouseButton button) const noexcept;

		/// @brief Gets the current position of the mouse
		/// @return The mouse position
		Vector2Int GetPosition() const noexcept { return _currentState.Position; }

		/// @brief Gets the amount the mouse has moved since the last tick
		/// @return The movement delta since last tick
		Vector2Int GetDelta() const noexcept { return _currentState.Position - _previousState.Position; }

		/// @brief Gets the amount the scroll wheel has moved since last tick
		/// @return The scroll wheel delta
		Vector2Int GetScrollWheelDelta() const noexcept { return _currentState.ScrollDelta; }

	private:
		/// @brief Processes all state changes since the last tick
		void ProcessCurrentState();

		/// @brief Saves the current state as the previous state
		void SavePreviousState() noexcept;
	};
}
