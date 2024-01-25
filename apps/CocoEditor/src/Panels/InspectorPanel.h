#pragma once

#include <functional>
#include <vector>

#include <Coco/ECS/Entity.h>
#include "../SelectionContext.h"

namespace Coco
{
	class InspectorPanel
	{
	public:
		struct ComponentInfo
		{
			string HeaderText;
			bool IsRemovable;

			std::function<void(ECS::Entity&)> DrawFunction;
			std::function<bool(const ECS::Entity&)> ComponentTestFunction;
			std::function<void(ECS::Entity&)> ComponentAdditionFunction;
			std::function<void(ECS::Entity&)> ComponentRemovalFunction;

			template<typename ComponentType>
			static ComponentInfo Create(const string& headerText, std::function<void(ComponentType&)> drawFunction, bool removable)
			{
				ComponentInfo info{};
				info.HeaderText = headerText;
				info.IsRemovable = removable;
				info.DrawFunction = [drawFunction](ECS::Entity& e) { ComponentType& c = e.GetComponent<ComponentType>(); drawFunction(c); };
				info.ComponentTestFunction = [](const ECS::Entity& e) { return e.HasComponent<ComponentType>(); };
				info.ComponentAdditionFunction = [](ECS::Entity& e) { e.AddComponent<ComponentType>(); };
				info.ComponentRemovalFunction = [](ECS::Entity& e) { e.RemoveComponent<ComponentType>(); };

				return info;
			}
		};

	public:
		InspectorPanel(SelectionContext& selectionContext);

		template<typename ComponentType>
		static void Register(const string& headerText, std::function<void(ComponentType&)> drawFunction, bool removable = true)
		{
			_registeredComponents.push_back(ComponentInfo::Create<ComponentType>(headerText, drawFunction, removable));
		}

		void Draw();

	private:
		static std::vector<ComponentInfo> _registeredComponents;

		SelectionContext& _selectionContext;

	private:
		void DrawEntityInspector(ECS::Entity& entity);
	};
}