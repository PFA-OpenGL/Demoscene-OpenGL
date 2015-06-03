#pragma once

#include <memory>
#include <vector>
#include <cstdint>

namespace AGE
{
	class BFCBlock;
	struct BFCRootType;

	class BFCBlockManager
	{
	public:
		static const std::size_t BlockMaxNumber = 256;
		void createItem(BFCRootType *drawable, std::uint8_t &blockID, std::uint8_t &itemId);
	private:
		std::size_t _blockCounter;
		std::vector<std::shared_ptr<BFCBlock>> _blocks;
	};
}