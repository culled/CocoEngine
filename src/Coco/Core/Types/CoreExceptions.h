#pragma once

#include "../Corepch.h"
#include "String.h"

namespace Coco
{
	class Exception : 
		public std::exception
	{
	public:
		Exception(const string& message);

		template<typename ... Args>
		Exception(const char* format, Args&& ... args) :
			Exception(FormatString(format, std::forward<Args>(args)...))
		{}
	};

	class NotFoundException :
		public Exception
	{
	public:
		NotFoundException(const string& message);
	};

	class InvalidOperationException :
		public Exception
	{
	public:
		InvalidOperationException(const string& message);
	};

	class OutOfRangeException :
		public Exception
	{
	public:
		OutOfRangeException(const string& message);
	};

	class NotImplementedException :
		public Exception
	{
	public:
		NotImplementedException();
		NotImplementedException(const string& message);
	};
}