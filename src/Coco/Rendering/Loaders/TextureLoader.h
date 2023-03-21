#pragma once

#include <Coco/Core/Resources/ResourceLoader.h>

namespace Coco::Rendering
{
	class TextureLoader final : public ResourceLoader
	{
	public:
		TextureLoader(const string& basePath);
		~TextureLoader() final = default;

		const char* GetResourceTypename() const noexcept final { return ResourceTypeToString(ResourceType::Texture); }

	protected:
		Ref<Resource> LoadImpl(const string& path) final;
		void SaveImpl(const Ref<Resource>& resource, const string& path) final;
	};
}