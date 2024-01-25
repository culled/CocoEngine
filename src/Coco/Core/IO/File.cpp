#include "Corepch.h"
#include "File.h"

#include "../Engine.h"

namespace Coco
{
	FileOpenException::FileOpenException(const string& message) :
		Exception(message)
	{}

	FileCreateException::FileCreateException(const string& message) :
		Exception(message)
	{}


	FileOperationException::FileOperationException(const string& message) :
		Exception(message)
	{}

	File::File(const FilePath& filePath, FileOpenFlags openFlags) :
		_filePath(filePath),
		_openFlags(openFlags),
		_fileStream(),
		_size(0),
		_position(0)
	{
		std::ios_base::openmode mode = 0;

		if ((openFlags & FileOpenFlags::Read) == FileOpenFlags::Read)
			mode |= std::ios::in;

		if ((openFlags & FileOpenFlags::Write) == FileOpenFlags::Write)
			mode |= std::ios::out;

		if ((openFlags & FileOpenFlags::Text) != FileOpenFlags::Text)
			mode |= std::ios::binary;

		if (mode == 0)
			throw FileOpenException("Invalid open mode. Please specify at least one FileOpenFlag");

		mode |= std::ios::ate;

		_fileStream = std::fstream(filePath._filePath, mode);

		if (!_fileStream.is_open())
			throw FileOpenException(FormatString("Unable to open file at \"{}\": {}", filePath.ToString(), IOStateToString(_fileStream.rdstate())));

		_size = _fileStream.tellg();
		_fileStream.seekg(0, std::ios::beg);
	}

	File::~File()
	{
		if (IsOpen())
			Close();
	}

	File File::Create(const FilePath& filePath, FileOpenFlags openFlags)
	{
		if (File::Exists(filePath))
			throw FileCreateException(FormatString("File at \"{}\" already exists", filePath.ToString()));

		return File(filePath, openFlags | FileOpenFlags::Write);
	}

	bool File::Exists(const FilePath& filePath)
	{
		return std::filesystem::exists(filePath._filePath);
	}

	string File::ReadAllText(const FilePath& filePath)
	{
		File f(filePath, FileOpenFlags::Read | FileOpenFlags::Text);
		string text = f.ReadTextToEnd();
		f.Close();

		return text;
	}

	std::vector<uint8> File::ReadAllBytes(const FilePath& filePath)
	{
		File f(filePath, FileOpenFlags::Read);
		std::vector<uint8> bytes = f.ReadToEnd();
		f.Close();

		return bytes;
	}

	void File::Seek(uint64 position, bool relative)
	{
		CheckFlags(FileOpenFlags::None);

		_fileStream.seekg(position, relative ? std::ios_base::cur : std::ios_base::beg);

		SyncState();
	}

	uint8 File::Peek()
	{
		CheckFlags(FileOpenFlags::Read);

		return static_cast<uint8>(_fileStream.peek());
	}

	uint64 File::Read(std::span<uint8> data)
	{
		CheckFlags(FileOpenFlags::Read);

		if (data.size() == 0)
			return 0;

		char* d = reinterpret_cast<char*>(data.data());

		_fileStream.read(d, data.size());
		SyncState();

		return _fileStream.gcount();
	}

	void File::PeekLine(string& outText, char lineTerminator)
	{
		CheckFlags(FileOpenFlags::Read | FileOpenFlags::Text);

		uint64 p = GetPosition();

		ReadLine(outText, lineTerminator);

		_fileStream.clear();
		Seek(p, false);

		CocoAssert(GetPosition() == p, "Failed to reset position after seek")
	}

	string File::ReadText(uint64 maxLength)
	{
		CheckFlags(FileOpenFlags::Read | FileOpenFlags::Text);

		if (maxLength == 0)
			return string();

		string text;
		text.resize(maxLength);

		_fileStream.read(text.data(), maxLength);
		SyncState();

		uint64 read = _fileStream.gcount();
		text.resize(read);

		return text;
	}

	bool File::ReadLine(string& outText, char lineTerminator)
	{
		CheckFlags(FileOpenFlags::Read | FileOpenFlags::Text);

		uint64 p = GetPosition();
		std::getline(_fileStream, outText, lineTerminator);
		SyncState();

		return GetPosition() - p != 0;
	}

	void File::Write(std::span<const uint8> data)
	{
		CheckFlags(FileOpenFlags::Write);

		const char* d = reinterpret_cast<const char*>(data.data());

		_fileStream.write(d, data.size());
		SyncState();
	}

	void File::Flush()
	{
		CheckFlags(FileOpenFlags::Write);

		_fileStream.flush();
	}

	void File::Write(const string& text)
	{
		CheckFlags(FileOpenFlags::Write | FileOpenFlags::Text);

		_fileStream.write(text.c_str(), text.length());
		SyncState();
	}

	void File::WriteLine(const string& text, char lineTerminator)
	{
		Write(text + lineTerminator);
	}

	string File::ReadTextToEnd()
	{
		CheckFlags(FileOpenFlags::Read | FileOpenFlags::Text);
		
		uint64 bytesLeft = GetSize() - GetPosition();
		return ReadText(bytesLeft);
	}

	std::vector<uint8> File::ReadToEnd()
	{
		CheckFlags(FileOpenFlags::Read);

		uint64 bytesLeft = GetSize() - GetPosition();
		std::vector<uint8> data(bytesLeft);
		Read(data);

		return data;
	}

	bool File::IsOpen() const
	{
		return _fileStream.is_open();
	}

	void File::Close()
	{
		if (!_fileStream.is_open())
			return;

		_fileStream.close();
	}

	void File::SyncState()
	{
		_position = _fileStream.tellg();
	}

	void File::CheckFlags(FileOpenFlags flags)
	{
		if (!IsOpen())
			throw FileOperationException("File is not open");

		CocoAssert(_fileStream, "File stream handle is null")

		if ((_openFlags & flags) != flags)
		{
			throw FileOperationException(
				FormatString(
					"File was not opened in the mode for the given operation: File flags = {}, required flags = {}",
					FileOpenFlagsToString(_openFlags),
					FileOpenFlagsToString(flags)
				)
			);
		}
	}
}