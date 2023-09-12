#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/MainLoop/TickListener.h>
#include "Keyboard.h"
#include "Mouse.h"

namespace Coco::Input
{
	/// @brief An EngineService that handles input from peripherals
	class InputService : public EngineService
	{
	public:
		/// @brief Priority for the tick handling input processing
		static constexpr int PreProcessTickPriority = -1000;

		/// @brief Priority for the tick handling late input processing
		static constexpr int PostProcessTickPriority = 1000;

	private:
		UniqueRef<Keyboard> _keyboard;
		UniqueRef<Mouse> _mouse;
		TickListener _preTickHandler;
		TickListener _postTickHandler;

	public:
		InputService();
		~InputService();

		/// @brief Gets the current keyboard
		/// @return The current keyboard
		Keyboard* GetKeyboard() { return _keyboard.get(); }

		/// @brief Gets the current keyboard
		/// @return The current keyboard
		const Keyboard* GetKeyboard() const { return _keyboard.get(); }

		/// @brief Gets the current mouse
		/// @return The current mouse
		Mouse* GetMouse() { return _mouse.get(); }

		/// @brief Gets the current mouse
		/// @return The current mouse
		const Mouse* GetMouse() const { return _mouse.get(); }

	private:
		/// @brief Tick for updating the current state of the peripherals
		/// @param tickInfo The info for the current tick
		void HandlePreTick(const TickInfo& tickInfo);

		/// @brief Tick for updating the previous state of the peripherals
		/// @param tickInfo The info for the current tick
		void HandlePostTick(const TickInfo& tickInfo);
	};
}