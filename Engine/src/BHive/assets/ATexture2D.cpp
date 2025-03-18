#include "ATexture2D.h"

namespace BHive
{
	ATexture2D::ATexture2D(const Ref<Texture2D> &texture)
		: TAsset(texture)
	{
	}

	void ATexture2D::Save(cereal::BinaryOutputArchive &ar) const
	{
		auto w = mObject->GetWidth();
		auto h = mObject->GetHeight();
		auto specs = mObject->GetSpecification();

		size_t data_size = w * h * specs.Channels;
		Buffer buffer(data_size);

		mObject->GetSubImage(0, 0, w, h, data_size, buffer.GetData());
		Asset::Save(ar);
		ar(MAKE_NVP("Width", w), MAKE_NVP("Height", h), MAKE_NVP("Specification", specs), MAKE_NVP("Data", buffer));

		buffer.Release();
	}

	void ATexture2D::Load(cereal::BinaryInputArchive &ar)
	{
		Buffer buffer;
		uint32_t w = 0, h = 0;
		FTextureSpecification specs{};

		Asset::Load(ar);
		ar(MAKE_NVP("Width", w), MAKE_NVP("Height", h), MAKE_NVP("Specification", specs), MAKE_NVP("Data", buffer));

		if (buffer)
		{
			mObject = CreateRef<Texture2D>(w, h, specs, buffer.GetData());
			buffer.Release();
		}
	}

	REFLECT(ATexture2D)
	{
		BEGIN_REFLECT(ATexture2D, "Texture2D").constructor();
	}

} // namespace BHive
