#pragma once

#include <Coco/Core/MainLoop/TickInfo.h>
#include <Coco/Core/IO/FileTypes.h>

namespace Coco
{
	class ContentPanel
	{
	public:
		void Update(const TickInfo& tick);

	private:
		void DrawFileNode(const FilePath& path);
	};
}