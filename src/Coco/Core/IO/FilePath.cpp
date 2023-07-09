#include "FilePath.h"

#include <regex>

namespace Coco
{
	FilePath::FilePath(const string& filePath) : 
		Path(filePath)
	{}

	string FilePath::GetFileName(const string& path, bool withExtension)
	{
		std::regex separator("[/\\]+?");
		std::smatch m;

		size_t startIndex;
		if (std::regex_search(path, m, separator))
			startIndex = m.position(m.size() - 1) + 1;
		else
			startIndex = 0;

		std::regex extension("\\.+?");
		size_t endIndex;
		if(!withExtension && std::regex_search(path, m, extension))
		{
			endIndex = m.position(m.size() - 1);
		}
		else
		{
			endIndex = path.length();
		}

		return path.substr(startIndex, endIndex - startIndex);
	}
}