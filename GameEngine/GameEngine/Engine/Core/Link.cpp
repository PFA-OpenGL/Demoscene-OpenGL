#include "Link.hpp"
#include <Core/Octree.hpp>

using namespace AGE;

void Link::registerCullableId(std::size_t id)
{
	for (auto &b : _cullableLinks)
	{
		if (b == std::size_t(-1))
		{
			b = id;
			return;
		}
	}
	assert(false);
}

void Link::unregisterCullableId(std::size_t id)
{
	for (auto &b : _cullableLinks)
	{
		if (b == id)
		{
			b = std::size_t(-1);
			return;
		}
	}
}


void Link::pushCommand()
{
	static_cast<Octree*>(_octree)->pushCommand(_position, _scale, _orientation, _cullableLinks);
}
const glm::mat4 &Link::getTransform()
{
	if (_computeTrans)
	{
		_trans = glm::mat4(1);
		_trans = glm::translate(_trans, _position);
		_trans = _trans * glm::toMat4(_orientation);
		_trans = glm::scale(_trans, _scale);
		_computeTrans = false;
	}
	return _trans;
}

		Link::Link()
		{
			reset();
		}

		void Link::reset()
		{
			_position = glm::vec3(0);
			_scale = glm::vec3(1);
			_orientation = glm::quat(glm::mat4(1));
			_trans = glm::mat4(1);
			_computeTrans = true;
			_cullableLinks.fill(std::size_t(-1));
		}
