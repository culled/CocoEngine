#include "FilePath.h"

#include <regex>
#include <Coco/Core/Engine.h>

namespace Coco
{
	FilePath::FilePath(const string& filePath) : 
		Path(filePath)
	{}

	string FilePath::GetFileName(const string& path, bool withExtension)
	{
		auto options = std::regex_constants::ECMAScript | std::regex_constants::icase;

		std::regex separator("[\\/]+?", options);
		std::smatch separatorMatch;

		string str = path;

		size_t startIndex = 0;
		while (std::regex_search(str, separatorMatch, separator))
		{
			startIndex += separatorMatch.position(0) + 1;
			str = separatorMatch.suffix();
		}

		std::regex extension("[.]+?", options);
		std::smatch extensionMatch;

		size_t endIndex = startIndex;
		if (!withExtension)
		{
			while (std::regex_search(str, extensionMatch, extension))
			{
				endIndex += extensionMatch.position(0);
				str = separatorMatch.suffix();
			}
		}

		if(endIndex == startIndex)
		{
			endIndex = path.length();
		}

		return path.substr(startIndex, endIndex - startIndex);
	}
}