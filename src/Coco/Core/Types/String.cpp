#include "Corepch.h"
#include "String.h"

namespace Coco
{
	wstring StringToWideString(const char* str)
	{
		size_t wStrSize = strlen(str) + 1;
		wstring wStr(wStrSize, L'#');

		mbstowcs_s(&wStrSize, wStr.data(), wStrSize, str, wStrSize - 1);

		wStr.resize(wStrSize - 1);
		return wStr;
	}

	string WideStringToString(const wchar_t* wStr)
	{
		size_t strSize = wcslen(wStr) + 1;
		string str(strSize, '#');

		wcstombs_s(&strSize, str.data(), strSize, wStr, strSize - 1);

		str.resize(strSize - 1);
		return str;
	}
}