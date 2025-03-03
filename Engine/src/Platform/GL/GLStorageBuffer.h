#include "gfx/StorageBuffer.h"
#include "GLBufferBase.h"

namespace BHive
{
	class GLStorageBuffer : public StorageBuffer, public GLBufferBase
	{
	public:
		GLStorageBuffer(uint32_t binding, size_t size);
		GLStorageBuffer(size_t size);
		GLStorageBuffer(const void *data, size_t size);

		virtual uint32_t GetRendererID() const override { return mBufferID; };

		virtual void BindBufferBase(size_t binding) override;
		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) override;
	};

} // namespace BHive
