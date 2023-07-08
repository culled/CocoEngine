#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	class COCOAPI StreamReader
	{
	protected:
		std::istream* _stream;

	public:
		StreamReader(std::istream& stream) noexcept;
		virtual ~StreamReader() = default;

		uint64_t GetPosition();
		void Seek(uint64_t position);
		void PeekLine(string& text, char lineEnd = '\n');
		bool ReadLine(string& text, char lineEnd = '\n');
	};
}