#pragma once

#include "BFCItemID.hpp"
#include <glm/glm.hpp>
#include <memory>

#include <Utils/MatrixConversion.hpp>

#include <glm/gtx/component_wise.hpp>

namespace AGE
{
	struct DRBData;

	// all engine cullable and drawable objects
	// inherit from this type
	struct BFCCullableObject
	{
	public:
		virtual ~BFCCullableObject() {}
		virtual CullableTypeID getBFCType() const = 0;
		virtual glm::vec4 setBFCTransform(const glm::mat4 &transformation);
		virtual const std::shared_ptr<DRBData> getDatas() const = 0;
	private:
		glm::mat4 _transform;
	};
}