#pragma once

#include <vector>

namespace AGE
{
	class Entity;
	struct BinaryEntityPack;

	void CreateBinaryEntityPack(BinaryEntityPack &pack, std::vector<Entity> &selection);
}