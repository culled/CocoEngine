#pragma once

#include <Coco/ECS/Entity.h>
#include <Coco/Core/Types/Size.h>

namespace Coco
{
	class ComponentUI;

	struct ComponentUIInstance
	{
		using TestFuncType = std::function<bool(const ECS::Entity&)>;
		using AddComponentFuncType = std::function<void(ECS::Entity&)>;
		using RemoveComponentFuncType = std::function<void(ECS::Entity&)>;

		TestFuncType TestFunc;
		AddComponentFuncType AddFunc;
		RemoveComponentFuncType RemoveFunc;
		UniqueRef<ComponentUI> UI;

		ComponentUIInstance(
			TestFuncType testFunc, 
			AddComponentFuncType addFunc,
			RemoveComponentFuncType removeFunc,
			UniqueRef<ComponentUI>&& ui) :
			TestFunc(testFunc),
			AddFunc(addFunc),
			RemoveFunc(removeFunc),
			UI(std::move(ui))
		{}
	};

	class ComponentUI
	{
	private:
		static std::vector<ComponentUIInstance> _sComponentUIInstances;

	public:
		virtual ~ComponentUI() = default;

		template<typename ComponentType, typename UIType, typename ... Args>
		static void RegisterComponentUI(Args&& ... args)
		{
			_sComponentUIInstances.push_back(
				ComponentUIInstance(
					[](const ECS::Entity& e) { return e.HasComponent<ComponentType>(); },
					[](ECS::Entity& e) { return e.AddComponent<ComponentType>(); },
					[](ECS::Entity& e) { return e.RemoveComponent<ComponentType>(); },
					CreateUniqueRef<UIType>(std::forward<Args>(args)...)
				)
			);
		}

		static void RegisterBuiltInComponentUIs();
		static bool DrawAddComponentUI(ECS::Entity& entity);
		static void DrawProperties(ECS::Entity& entity);
		static void DrawGizmos(ECS::Entity& entity, const SizeInt& viewportSize);

	protected:
		virtual const char* GetHeader() const = 0;
		virtual bool IsRemovable() const { return true; }
		virtual void DrawPropertiesImpl(ECS::Entity& entity) = 0;
		virtual void DrawGizmosImpl(ECS::Entity& entity, const SizeInt& viewportSize) = 0;
	};
}