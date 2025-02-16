#include "gfx/StorageBuffer.h"

namespace BHive
{
	class GLStorageBuffer : public StorageBuffer
	{
	private:
		/* data */
	public:
		GLStorageBuffer(uint32_t binding, size_t size);
		~GLStorageBuffer();

		virtual uint32_t GetRendererID() const override { return mBufferID; };
		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) override;

	private:
		uint32_t mBufferID = 0;
	};

} // namespace BHive
