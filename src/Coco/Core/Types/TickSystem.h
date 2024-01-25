#pragma once

#include "../Corepch.h"
#include "Refs.h"

namespace Coco
{
	/// @brief A general system for dispatching ticks
	/// @tparam TickListenerType The type of tick listener
	/// @tparam ...TickArgs The arguments for the tick
	template<typename TickListenerType, typename ... TickArgs>
	class TickSystem
	{
	public:
		/// @brief The type of function for sorting tick listeners
		using SortingFunc = std::function<bool(const Ref<TickListenerType>&, const Ref<TickListenerType>&)>;

		/// @brief The type of function for dispatching ticks
		using TickFunc = std::function<void(Ref<TickListenerType>&, TickArgs&& ...)>;

		/// @brief The type of function for handling errors during a tick
		using TickErrorFunc = std::function<bool(const std::exception&)>;

	public:
		TickSystem(SortingFunc sortFunction, TickFunc tickFunction, TickErrorFunc errorFunction) :
			_sortFunction(sortFunction),
			_tickFunction(tickFunction),
			_errorFunction(errorFunction),
			_tickListeners(),
			_listenersNeedSorting(false)
		{}

		~TickSystem()
		{
			_tickListeners.clear();
		}

		/// @brief Adds a tick listener to this system
		/// @param listener The listener
		void AddTickListener(Ref<TickListenerType> listener)
		{
			const auto it = std::find_if(_tickListeners.cbegin(), _tickListeners.cend(), 
				[listener](const Ref<TickListenerType>& other)
				{
					return other.Get() == listener.Get();
				});

			if (it != _tickListeners.cend())
				return;

			_tickListeners.push_back(listener);
			_listenersNeedSorting = true;
		}

		/// @brief Removes a tick listener from this system
		/// @param listener The listener
		void RemoveTickListener(Ref<TickListenerType> listener)
		{
			const auto it = std::find_if(_tickListeners.cbegin(), _tickListeners.cend(), 
				[listener](const Ref<TickListenerType>& other)
				{
					return other.Get() == listener.Get();
				});

			if (it == _tickListeners.cend())
				return;

			_tickListeners.erase(it);
		}

		/// @brief Sorts all tick listeners
		void SortTickListeners()
		{
			if (!_listenersNeedSorting)
				return;

			std::sort(_tickListeners.begin(), _tickListeners.end(), _sortFunction);
			_listenersNeedSorting = false;
		}

		/// @brief Ticks all listeners
		/// @param ...tickArgs The arguments to pass to each tick listener
		void TickAllListeners(TickArgs&& ... tickArgs)
		{
			if (_listenersNeedSorting)
				SortTickListeners();

			bool needsCleanup = false;

			std::vector<Ref<TickListenerType>> tempListeners(_tickListeners);
			for (auto it = tempListeners.begin(); it != tempListeners.end(); it++)
			{
				if (!it->IsValid())
				{
					needsCleanup = true;
					continue;
				}

				try
				{
					_tickFunction(*it, std::forward<TickArgs>(tickArgs)...);
				}
				catch (const std::exception& ex)
				{
					if (!_errorFunction(ex))
						throw;
				}
			}

			if (needsCleanup)
				CleanupInvalidListeners();
		}

		/// @brief Cleans up all invalid tick listeners
		void CleanupInvalidListeners()
		{
			auto it = _tickListeners.begin();

			while (it != _tickListeners.end())
			{
				if (it->IsValid())
				{
					++it;
				}
				else
				{
					it = _tickListeners.erase(it);
				}
			}
		}

	private:
		SortingFunc _sortFunction;
		TickFunc _tickFunction;
		TickErrorFunc _errorFunction;
		std::vector<Ref<TickListenerType>> _tickListeners;
		bool _listenersNeedSorting;
	};
}