#include "Size.h"

#include "Vector.h"

namespace Coco
{
	const SizeInt SizeInt::Zero = SizeInt(0, 0);

	SizeInt::SizeInt(int width, int height) noexcept : Width(width), Height(height)
	{}

	SizeInt SizeInt::Parse(const string & str)
	{
		uint64_t currentCharacterIndex = 0;
		uint64_t fieldIndex = 0;

		SizeInt value;

		while (currentCharacterIndex < str.length() && fieldIndex < 2)
		{
			uint64_t endIndex = str.find_first_of('x', currentCharacterIndex);

			if (endIndex == string::npos)
				endIndex = str.length();

			string part = str.substr(currentCharacterIndex, endIndex);
			int v = atoi(part.c_str());

			if (fieldIndex == 0)
				value.Width = v;
			else
				value.Height = v;

			currentCharacterIndex = endIndex + 1;
			fieldIndex++;
		}

		return value;
	}

	SizeInt::operator Vector2Int() const noexcept
	{
		return Vector2Int(Width, Height);
	}

	SizeInt::operator Vector2() const noexcept
	{
		return Vector2(Width, Height);
	}

	const Size Size::Zero = Size(0.0, 0.0);

	Size::Size(double width, double height) noexcept : Width(width), Height(height)
	{}

	Size Size::Parse(const string & str)
	{
		uint64_t currentCharacterIndex = 0;
		uint64_t fieldIndex = 0;

		Size value;

		while (currentCharacterIndex < str.length() && fieldIndex < 2)
		{
			uint64_t endIndex = str.find_first_of('x', currentCharacterIndex);

			if (endIndex == string::npos)
				endIndex = str.length();

			string part = str.substr(currentCharacterIndex, endIndex);
			double v = atof(part.c_str());

			if (fieldIndex == 0)
				value.Width = v;
			else
				value.Height = v;

			currentCharacterIndex = endIndex + 1;
			fieldIndex++;
		}

		return value;
	}

	Size::operator Vector2() const noexcept
	{
		return Vector2(Width, Height);
	}
}