#pragma once

#include "Inspector.h"

namespace BHive
{
	struct Inspector_String : public Inspector
	{
		virtual bool Inspect(FPropertyData &property_data, const bool is_read_only) override;

		REFLECTABLEV(Inspector)
	};

	struct Inspector_Float : public Inspector
	{
		virtual bool Inspect(FPropertyData &property_data, const bool is_read_only) override;

		REFLECTABLEV(Inspector)
	};

	struct Inspector_Bool : public Inspector
	{
		virtual bool Inspect(FPropertyData &property_data, const bool is_read_only) override;

		REFLECTABLEV(Inspector)
	};

	template <typename TIntegerType>
	struct Inspector_Int : public Inspector
	{
		virtual bool Inspect(FPropertyData &property_data, const bool is_read_only) override;

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(Inspector_String);
	REFLECT_EXTERN(Inspector_Float);
	REFLECT_EXTERN(Inspector_Bool);

	REFLECT_EXTERN(Inspector_Int<uint8_t>);
	REFLECT_EXTERN(Inspector_Int<uint16_t>);
	REFLECT_EXTERN(Inspector_Int<uint32_t>);
	REFLECT_EXTERN(Inspector_Int<uint64_t>);

	REFLECT_EXTERN(Inspector_Int<int8_t>);
	REFLECT_EXTERN(Inspector_Int<int16_t>);
	REFLECT_EXTERN(Inspector_Int<int32_t>);
	REFLECT_EXTERN(Inspector_Int<int64_t>);

} // namespace BHive
