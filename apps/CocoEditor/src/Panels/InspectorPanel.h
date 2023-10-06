#pragma once

#include "../ComponentUI/ComponentUI.h"
#include <Coco/ECS/Entity.h>

namespace Coco
{
	struct TickInfo;
	class SelectionContext;

	struct ComponentUIContainer
	{
		using TestFuncType = std::function<bool(const ECS::Entity&)>;
		TestFuncType TestFunc;
		UniqueRef<ComponentUI> UI;

		ComponentUIContainer(TestFuncType testFunc, UniqueRef<ComponentUI>&& ui) :
			TestFunc(testFunc),
			UI(std::move(ui))
		{}
	};

	class InspectorPanel
	{
	private:
		static std::vector<ComponentUIContainer> _sComponentUIs;

		SelectionContext& _selection;

	public:
		InspectorPanel();

		template<typename ComponentType, typename UIType, typename ... Args>
		static void RegisterComponentUI(Args&& ... args)
		{
			_sComponentUIs.push_back(
				ComponentUIContainer(
					[](const ECS::Entity& e) { return e.HasComponent<ComponentType>(); },
					CreateUniqueRef<UIType>(std::forward<Args>(args)...)
				)
			);
		}

		void Update(const TickInfo& tickInfo);

	private:
		void RegisterDefaultComponentUIs();
		void DrawEntityInspector();
	};
}