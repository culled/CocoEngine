#pragma once

#include "Inputpch.h"
#include "InputTypes.h"
#include <Coco/Core/Events/Event.h>
#include <Coco/Core/Types/Vector.h>

namespace Coco::Input
{
	/// @brief A state for a mouse
	struct MouseState
	{
		/// @brief The maximum number of mouse buttons
		static constexpr uint8 ButtonCount = 6;

		/// @brief Mouse button states
		std::array<bool, ButtonCount> ButtonStates;

		/// @brief The mouse position
		Vector2Int Position;

		/// @brief The mouse move delta
		Vector2Int MoveDelta;

		/// @brief The mouse scroll delta
		Vector2Int ScrollDelta;

		MouseState();
	};

	/// @brief Possible state changes for a mouse
	struct MouseStateChange
	{
		/// @brief The button (if any) whose state changed
		std::optional<MouseButton> Button;

		/// @brief If true, the button for this state was pressed, else it was released
		bool IsButtonPressed;

		/// @brief The new mouse position (if any)
		std::optional<Vector2Int> Position;

		/// @brief The new mouse move delta (if any)
		std::optional<Vector2Int> MoveDelta;

		/// @brief The mouse scroll delta (if any)
		std::optional<Vector2Int> ScrollDelta;

		MouseStateChange();

		/// @brief Creates a mouse state change for moving the mouse
		/// @param newPosition The new mouse position
		/// @return A mouse state change
		static MouseStateChange PositionStateChange(const Vector2Int& newPosition);

		/// @brief Creates a mouse state change for moving the mouse, but the position may not have changed (i.e. the mouse is confined)
		/// @param delta The move delta
		/// @return A mouse state change
		static MouseStateChange MoveDeltaStateChange(const Vector2Int& delta);

		/// @brief Creates a mouse state change for scrolling the mouse
		/// @param scrollDelta The amount the mouse was scrolled
		/// @return A mouse state change
		static MouseStateChange ScrollStateChange(const Vector2Int& scrollDelta);

		/// @brief Creates a mouse state change for pressing or releasing a mouse button
		/// @param button The mouse button
		/// @param isPressed True if the button was pressed, false if it was released
		/// @return A mouse state change
		static MouseStateChange ButtonStateChange(MouseButton button, bool isPressed);
	};

	/// @brief A mouse input device
	class Mouse
	{
		friend class InputService;

	public:
		/// @brief Invoked when a mouse button is pressed
		Event<MouseButton> OnButtonPressed;

		/// @brief Invoked when a mouse button is released
		Event<MouseButton> OnButtonReleased;

		/// @brief Invoked when a mouse button is double-clicked
		//Event<MouseButton> OnDoubleClicked; // TODO: handle double clicks

		/// @brief Invoked when the mouse's position changed
		Event<const Vector2Int&> OnPositionChanged;

		/// @brief Invoked when the mouse is moved
		Event<const Vector2Int&> OnMoved;

		/// @brief Invoked when the mouse is scrolled
		Event<const Vector2Int&> OnScrolled;

	private:
		std::vector<MouseStateChange> _preProcessStateChanges;
		MouseState _currentState;
		MouseState _previousState;

		bool _isFirstState;

	public:
		Mouse();
		~Mouse() = default;

		/// @brief Updates the pressed state for a mouse button
		/// @param button The mouse button
		/// @param isPressed True if the mouse button is pressed
		void UpdateButtonState(MouseButton button, bool isPressed);

		/// @brief Updates the position state for the mouse
		/// @param newPosition The new mouse position
		void UpdatePositionState(const Vector2Int& newPosition);

		/// @brief Updates the move delta for the mouse
		/// @param moveDelta The move delta
		void UpdateMoveDeltaState(const Vector2Int& moveDelta);

		/// @brief Updates the scroll wheel state for the mouse
		/// @param scrollDelta The scroll delta
		void UpdateScrollState(const Vector2Int& scrollDelta);

		/// @brief Clears the states of all buttons
		void ClearAllButtonStates();

		/// @brief Called when a mouse button has been double-clicked
		/// @param button The mouse button
		//void DoubleClicked(MouseButton button);

		/// @brief Determines if the given mouse button is currently pressed
		/// @param button The mouse button
		/// @return True if the mouse button is currently pressed
		bool IsButtonPressed(MouseButton button) const;

		/// @brief Determines if the given mouse button was just pressed (unpressed -> pressed) within the last tick
		/// @param button The mouse button
		/// @return True if the mouse button was pressed since the last tick
		bool WasButtonJustPressed(MouseButton button) const;

		/// @brief Determines if the given mouse button was just released (pressed -> unpressed) within the last tick
		/// @param button The mouse button
		/// @return True if the mouse button was released since the last tick
		bool WasButtonJustReleased(MouseButton button) const;

		/// @brief Gets the current position of the mouse, in screen-coordinates
		/// @return The mouse position
		Vector2Int GetPosition() const { return _currentState.Position; }

		/// @brief Gets the amount the mouse's position has changed since the last tick
		/// @return The position delta since last tick
		Vector2Int GetPositionDelta() const { return _currentState.Position - _previousState.Position; }

		/// @brief Gets the amount the mouse has moved since the last tick
		/// @return The movement delta since last tick
		Vector2Int GetMoveDelta() const { return _currentState.MoveDelta; }

		/// @brief Gets the amount the scroll wheel has moved since last tick
		/// @return The scroll wheel delta
		Vector2Int GetScrollWheelDelta() const { return _currentState.ScrollDelta; }

	private:
		/// @brief Gets all state changes since the last tick
		/// @return The state changes
		std::span<const MouseStateChange> GetStateChanges() const { return _preProcessStateChanges; }

		/// @brief Saves the current state as the previous state
		void SavePreviousState();
	};
}