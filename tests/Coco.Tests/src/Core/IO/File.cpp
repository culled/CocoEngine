#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/IO/File.h>
#include <filesystem>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::IO::File
{
	TEST_CLASS(FileTests)
	{
		const string testFile = "files/test.txt";
		const string testLinesFile = "files/testlines.txt";
		const string testWriteFile = "files/testwrite.txt";

		TEST_METHOD(OpenCloseFile)
		{
			std::filesystem::path fullPath = std::filesystem::absolute(testFile);

			::File file = ::File::Open(fullPath.string(), FileModeFlags::Read);
			Assert::IsTrue(file.IsValid());
			Assert::AreEqual(file.GetSize(), static_cast<uint64_t>(12));

			file.Close();
			Assert::IsFalse(file.IsValid());
		}

		TEST_METHOD(ReadAllTextFromFile)
		{
			string text = ::File::ReadAllText(testFile);
			Assert::AreEqual(text, string("Hello World!"));
		}

		TEST_METHOD(ReadTextLinesFromFile)
		{
			::File file = ::File::Open(testLinesFile, FileModeFlags::Read);

			string line1;
			file.ReadLine(line1);
			Assert::AreEqual(line1, string("Line 1"));

			string line2;
			file.ReadLine(line2);
			Assert::AreEqual(line2, string("Line 2"));

			string remaining = file.ReadTextToEnd();
			Assert::AreEqual(remaining, string("Line 3"));

			file.Close();
			Assert::IsFalse(file.IsValid());
		}

		TEST_METHOD(WriteTextToFile)
		{
			::File writeFile = ::File::Open(testWriteFile, FileModeFlags::Write);
			Assert::AreEqual(writeFile.GetPosition(), static_cast<uint64_t>(0));

			string text("This is a write test.");
			writeFile.WriteText(text);
			writeFile.Close();

			string readText = ::File::ReadAllText(testWriteFile);
			Assert::AreEqual(readText, text);
		}
	};
}