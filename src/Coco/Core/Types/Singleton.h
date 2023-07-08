#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	/// @brief Provides singleton access to a class
	/// @tparam Type The class type
	template<typename Type>
	class COCOAPI Singleton
	{
	private:
		static Type* _instance;

	protected:
		Singleton() = default;

		/// @brief Sets the singleton instance
		/// @param instance The singleton instance
		void SetSingleton(Type* instance) noexcept
		{
			_instance = instance;
		}

	public:
		virtual ~Singleton() = default;

		/// @brief Gets the singleton instance (if one has been set)
		/// @returns The singleton instance 
		static Type* Get() noexcept
		{
			return _instance;
		}
	};

	template<typename Type>
	Type* Singleton<Type>::_instance = nullptr;
}