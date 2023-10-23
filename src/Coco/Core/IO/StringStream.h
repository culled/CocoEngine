#pragma once

#include "TextStream.h"

namespace Coco
{
	/// @brief A stream that can read from and write to a string
	class StringStream : public InputTextStream, public OutputTextStream
	{
	private:
		std::stringstream _stream;
		uint64 _position;

	public:
		StringStream(const string& str);

		uint64 GetPosition() const final { return _position; }
		void Seek(uint64 position, bool relative) final;
		char PeekChar() final;
		void PeekLine(string& outText, char lineTerminator) final;
		string ReadText(uint64 maxLength) final;
		bool ReadLine(string& outText, char lineTerminator) final;
		void Write(const string& text) final;
		void WriteLine(const string& text, char lineTerminator) final;
		void Flush() final;

		string ToString();

	private:
		/// @brief Synchronises this streams's state with the underlying state
		void SyncState();
	};
}