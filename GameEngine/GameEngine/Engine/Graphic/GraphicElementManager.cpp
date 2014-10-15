#include "GraphicElementManager.hpp"

#include "BFC/BFCCullableHandle.hpp"

#include "AssetManagement/Instance/MeshInstance.hh"

#include "BFC/BFCBlockManager.hpp"

namespace AGE
{
	struct MeshInstance;

	BFCCullableHandle GraphicElementManager::addMesh(std::shared_ptr<MeshInstance> meshInstance)
	{
		DRBMesh *drbMesh = _meshPool.create();
		drbMesh->subMeshs = meshInstance->subMeshs;

		BFCCullableHandle result;
		
		//_bfcBlockManager->createItem(drbMesh, result.blockId, result.itemId);

		// We don't set the type in the key, it's not the job of GraphicElementManager,
		// it has to be done by the client who ask for the creation

		//result.elementPtr = drbMesh;
		return result;
	}
}