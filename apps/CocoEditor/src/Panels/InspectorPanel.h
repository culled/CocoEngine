#pragma once

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