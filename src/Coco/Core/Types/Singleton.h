#pragma once

namespace Coco
{
	/// @brief Provides singleton access to a class
	/// @tparam ClassType The type of class
	template<typename ClassType>
	class Singleton
	{
	private:
		static ClassType* _sInstance;

	protected:
		Singleton()
		{
			_sInstance = static_cast<ClassType*>(this);
		}

		~Singleton()
		{
			_sInstance = nullptr;
		}

	public:
		/// @brief Gets the singleton instance
		/// @return The singleton instance
		static ClassType* Get() { return _sInstance; }

		/// @brief Gets the singleton instance
		/// @return The singleton instance
		static const ClassType* cGet() { return _sInstance; }
	};

	template<typename ClassType>
	ClassType* Singleton<ClassType>::_sInstance = nullptr;
}