#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/Types/Singleton.h>
#include <Coco/Core/MainLoop/TickListener.h>
#include "Keyboard.h"
#include "Mouse.h"
#include "InputLayer.h"
#include "UnhandledInputLayer.h"

namespace Coco::Input
{
	/// @brief An EngineService that handles input from peripherals
	class InputService : public EngineService, public Singleton<InputService>
	{
	public:
		/// @brief Priority for the tick handling input processing
		static constexpr int ProcessTickPriority = -100;

		/// @brief Priority for the tick handling late input processing
		static constexpr int PostProcessTickPriority = 1000;

	private:
		UniqueRef<Keyboard> _keyboard;
		UniqueRef<Mouse> _mouse;
		ManagedRef<TickListener> _processTickHandler;
		ManagedRef<TickListener> _postTickHandler;
		std::vector<Ref<InputLayer>> _inputLayers;
		ManagedRef<UnhandledInputLayer> _unhandledInputLayer;
		bool _inputLayersNeedSorting;

	public:
		InputService();
		~InputService();

		/// @brief Gets the current keyboard
		/// @return The current keyboard
		Keyboard& GetKeyboard() { return *_keyboard; }

		/// @brief Gets the current keyboard
		/// @return The current keyboard
		const Keyboard& GetKeyboard() const { return *_keyboard; }

		/// @brief Gets the current mouse
		/// @return The current mouse
		Mouse& GetMouse() { return *_mouse; }

		/// @brief Gets the current mouse
		/// @return The current mouse
		const Mouse& GetMouse() const { return *_mouse; }

		/// @brief Registers an input layer
		/// @param layer The input layer
		void RegisterInputLayer(Ref<InputLayer> layer);

		/// @brief Unregisters an input layer
		/// @param layer The input layer
		void UnregisterInputLayer(Ref<InputLayer> layer);

		/// @brief Gets the unhandled input layer
		/// @return The unhandled input layer
		Ref<UnhandledInputLayer> GetUnhandledInputLayer() const { return _unhandledInputLayer; }

		/// @brief Causes all input devices to reset their state. Mainly should be called if the application looses focus
		void LostFocus();

	private:
		/// @brief Tick for updating the current state of the peripherals
		/// @param tickInfo The info for the current tick
		void HandleProcessTick(const TickInfo& tickInfo);

		/// @brief Sorts input layers by their priorites
		void SortInputLayers();

		/// @brief Tick for updating the previous state of the peripherals
		/// @param tickInfo The info for the current tick
		void HandlePostTick(const TickInfo& tickInfo);
	};
}