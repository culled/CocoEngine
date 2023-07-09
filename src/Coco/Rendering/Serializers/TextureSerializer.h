#pragma once

#include <Coco/Core/Resources/Serializers/KeyValueResourceSerializer.h>
#include "../Texture.h"

namespace Coco::Rendering
{
	/// @brief A serializer for texture files (*.ctexture)
	class TextureSerializer final : public KeyValueResourceSerializer
	{
	private:
		static constexpr const char* s_nameVariable = "name";
		static constexpr const char* s_imageFileVariable = "imageFile";
		static constexpr const char* s_usageFlagsVariable = "usageFlags";
		static constexpr const char* s_filterModeVariable = "filterMode";
		static constexpr const char* s_repeatModeVariable = "repeatMode";
		static constexpr const char* s_maxAnisotropyVariable = "maxAnisotropy";
		static constexpr const char* s_channelCountVariable = "channelCount";

	public:
		TextureSerializer() = default;
		~TextureSerializer() final = default;

		DefineSerializerResourceType(Texture)

		string Serialize(ResourceLibrary& library, const Ref<Resource>& resource) final;
		void Deserialize(ResourceLibrary& library, const string& data, Ref<Resource> resource) final;
	};
}