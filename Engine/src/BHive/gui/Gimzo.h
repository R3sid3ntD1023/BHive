#pragma once

#include "core/Core.h"

namespace BHive
{
	struct BHIVE_API GizmoContext
	{
		static void SetOrthographic(bool ortho);

		static void SetRect(float x, float y, float w, float h);

		static bool IsUsing();
	};

	struct BHIVE_API ViewGizmo
	{
		void ViewManipulate(glm::mat4 &view, float distance, const glm::vec2 &pos, const glm::vec2 &size, int32_t bg_color);

		bool IsUsing() const;

		operator bool() const { return IsUsing(); }
	};

	struct BHIVE_API Gizmo
	{
		bool Manipulate(const glm::mat4 &view, const glm::mat4 &projection, glm::mat4 &transform, glm::mat4 &delta, const glm::vec3 &snapping, float *bounds = nullptr);

		void SetOperationAndMode(int32_t operation, int32_t mode);

		bool IsUsing() const;

		operator bool() const { return IsUsing(); }

	private:
		int32_t mOperation;
		int32_t mMode;
	};
} // namespace BHive