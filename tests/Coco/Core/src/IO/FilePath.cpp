#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/IO/FilePath.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::Core::IO
{
	TEST_CLASS(FilePathTests)
	{
		TEST_METHOD(AbsolutePathEquality)
		{
			FilePath path1("C:\\A\\B\\C.txt");
			FilePath path2("C:/A/B/C.txt");

			Assert::IsTrue(path1 == path2);
		}

		TEST_METHOD(RelativePathEquality)
		{
			FilePath path1("A\\B\\C.txt");
			FilePath path2("A/B/C.txt");

			Assert::IsTrue(path1 == path2);
		}

		TEST_METHOD(ConcatPaths)
		{
			FilePath path("A\\B");

			path /= "C.txt";

			FilePath fullPath("A\\B\\C.txt");

			Assert::IsTrue(path == fullPath);
		}

		TEST_METHOD(AbsoluteToRelativePath)
		{
			FilePath absPath("C:\\A\\B\\C.txt");
			FilePath rootPath("C:\\A");

			Assert::IsFalse(absPath.IsRelative());

			FilePath relativePath = FilePath::GetRelativePath(absPath, rootPath);

			Assert::IsTrue(relativePath == FilePath("B\\C.txt"));
			Assert::IsTrue(relativePath.IsRelative());
		}

		TEST_METHOD(CombineToPath)
		{
			FilePath path = FilePath::CombineToPath("C:\\A", "B", "C.txt");

			Assert::IsTrue(path == FilePath("C:\\A\\B\\C.txt"));
		}

		TEST_METHOD(GetFileName)
		{
			FilePath path("C:\\A\\B\\C.A.B.txt");

			string fileNameWithExt = path.GetFileName(true);
			Assert::IsTrue(fileNameWithExt == "C.A.B.txt");

			string fileNameWithoutExt = path.GetFileName(false);
			Assert::IsTrue(fileNameWithoutExt == "C.A.B");
		}

		TEST_METHOD(GetFileExtension)
		{
			FilePath path("C:\\A\\B\\C.A.B.txt");

			string extension = path.GetExtension();
			Assert::IsTrue(extension == ".txt");
		}

		TEST_METHOD(GetParentDirectory)
		{
			FilePath path("C:\\A\\B\\C.txt");
			FilePath parent = path.GetParentDirectory();

			Assert::IsFalse(path.IsDirectory());
			Assert::IsTrue(parent == FilePath("C:\\A\\B"));
			Assert::IsTrue(parent.IsDirectory());
		}

		TEST_METHOD(ChangeExtension)
		{
			FilePath path("C:\\A\\B\\C.A.B.txt");
			FilePath newExt = path.ChangeExtension("abc");

			string extension = newExt.GetExtension();
			Assert::IsTrue(extension == ".abc");
			Assert::IsTrue(path.GetFileName(false) == newExt.GetFileName(false));
		}

		TEST_METHOD(IsEmpty)
		{
			FilePath path;

			Assert::IsTrue(path.IsEmpty());

			path = "A/B.txt";

			Assert::IsFalse(path.IsEmpty());
		}
	};
}