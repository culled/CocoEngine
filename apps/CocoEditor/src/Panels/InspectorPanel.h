#pragma once

#include <Coco/ECS/Entity.h>

namespace Coco
{
	struct TickInfo;
	class SelectionContext;

	class InspectorPanel
	{
	private:
		SelectionContext& _selection;

	public:
		InspectorPanel();

		void Update(const TickInfo& tickInfo);

	private:
		void DrawEntityInspector();
	};
}