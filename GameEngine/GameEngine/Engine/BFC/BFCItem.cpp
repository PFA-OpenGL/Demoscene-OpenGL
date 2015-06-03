#include "BFCItem.hpp"

namespace AGE
{
	BFCItem::BFCItem()
		: _drawable(nullptr)
		, _transformationInfos(nullptr)
		, _position(glm::vec4(0,0,0,0))
	{

	}

	void BFCItem::setDrawable(BFCRootType *drawable)
	{
		_drawable = drawable;
	}
}