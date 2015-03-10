#include "RigidBody.hpp"
#include <Core/AScene.hh>
#include <Physic/BulletDynamicManager.hpp>
#include <Physic/DynamicMotionState.hpp>

namespace AGE
{
	RigidBody::RigidBody()
		: ComponentBase(),
		_collisionShape(nullptr),
		_motionState(nullptr),
		_rigidBody(nullptr),
		_manager(nullptr),
		_mass(0.0f),
		_inertia(btVector3(0.0f, 0.0f, 0.0f)),
		_rotationConstraint(glm::vec3(1, 1, 1)),
		_transformConstraint(glm::vec3(1, 1, 1)),
		_shapeType(UndefinedTypeId),
		_shapePath(""),
		_collisionShapeType(UNDEFINED)
	{
	}

	void RigidBody::init(AScene *scene, float mass/* = 1.0f*/)
	{
		_manager = dynamic_cast<BulletDynamicManager*>(scene->getInstance<BulletCollisionManager>());
		assert(_manager != nullptr);
		_mass = mass;
	}

	void RigidBody::reset(AScene *scene)
	{
		_clearBulletObjects();

		_shapeType = UNDEFINED;
		_mass = 0.0f;
		_rotationConstraint = glm::vec3(1, 1, 1);
		_transformConstraint = glm::vec3(1, 1, 1);
		_inertia.setValue(0, 0, 0);
	}

	void RigidBody::setTransformation(const AGE::Link *link)
	{
		btTransform tt = _rigidBody->getCenterOfMassTransform();
		tt.setOrigin(convertGLMVectorToBullet(link->getPosition()));
		glm::quat rot = link->getOrientation();
		tt.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
		_rigidBody->setWorldTransform(tt);
		_collisionShape->setLocalScaling(convertGLMVectorToBullet(link->getScale()));
	}

	btMotionState &RigidBody::getMotionState()
	{
		assert(_motionState != nullptr && "Motion state is NULL, RigidBody error. Tips : Have you setAcollisionShape to Component ?.");
		return *_motionState;
	}

	btCollisionShape &RigidBody::getShape()
	{
		assert(_collisionShape != nullptr && "Shape is NULL, RigidBody error. Tips : Have you setAcollisionShape to Component ?.");
		return *_collisionShape;
	}

	btRigidBody &RigidBody::getBody()
	{
		assert(_rigidBody != nullptr && "RigidBody is NULL. Tips : Have you setAcollisionShape to Component ?.");
		return *_rigidBody;
	}

	void RigidBody::setMass(float mass)
	{
		_mass = btScalar(mass);
	}

	int RigidBody::getMass() const
	{
		return _mass;
	}

	void RigidBody::setInertia(const glm::vec3 &inertia)
	{
		_inertia = convertGLMVectorToBullet(inertia);
	}

	void RigidBody::setCollisionMesh(AScene *scene
		, const Entity &entity
		, const std::string &meshPath
		, short filterGroup /*= 1*/
		, short filterMask /*= -1*/)
	{
		_clearBulletObjects();

		_shapePath = meshPath;

		auto e = entity;
		_motionState = _manager->getObjectPool().create<DynamicMotionState>(&this->entity.getLink());
		auto media = _manager->loadShape(meshPath);
		if (!media)
			return;
		auto s = dynamic_cast<btConvexHullShape*>(media.get());
		if (s) // dynamic
		{
			_collisionShape = _manager->getObjectPool().create<btConvexHullShape>(*s);
			_shapeType = TypeID::Get<btConvexHullShape>();
		}
		else // static
		{
			auto m = dynamic_cast<btBvhTriangleMeshShape*>(media.get());
			_collisionShape = _manager->getObjectPool().create<btScaledBvhTriangleMeshShape>(m, btVector3(1, 1, 1));
			_shapeType = TypeID::Get<btScaledBvhTriangleMeshShape>();
		}
		
		if (_mass != 0)
			_collisionShape->calculateLocalInertia(_mass, _inertia);
		if (((size_t)(this) & 0x3) == 0)
		{
			int i = 0;
		}
		else
		{
		//	assert(false);
		}
		_rigidBody = _manager->getObjectPool().create<btRigidBody>(_mass, _motionState, _collisionShape, _inertia);
		_rigidBody->setUserPointer((void*)(entity.getPtr()));
		_rigidBody->setAngularFactor(convertGLMVectorToBullet(_rotationConstraint));
		_rigidBody->setLinearFactor(convertGLMVectorToBullet(_transformConstraint));

		if (_rigidBody->isStaticObject())
		{
			//_rigidBody->setActivationState(DISABLE_SIMULATION);
		}
		_manager->getWorld()->addRigidBody(_rigidBody, filterGroup, filterMask);
		setTransformation(&e.getLink());
	}

	void RigidBody::setCollisionShape(
		AScene *scene
		, const Entity &entity
		, CollisionShape c
		, short filterGroup /*= 1*/
		, short filterMask /* = -1*/)
	{
		if (c == UNDEFINED)
			return;
		_shapePath = "";
		_clearBulletObjects();

		auto e = entity;
		_motionState = _manager->getObjectPool().create<DynamicMotionState>(&e.getLink());
		if (c == BOX)
		{
			_collisionShape = _manager->getObjectPool().create<btBoxShape>(btVector3(0.5, 0.5, 0.5));
			_shapeType = TypeID::Get<btBoxShape>();
		}
		else if (c == SPHERE)
		{
			_collisionShape = _manager->getObjectPool().create<btSphereShape>(btSphereShape(1));
			_shapeType = TypeID::Get<btSphereShape>();
		}
		else
		{
			assert(false);
		}

		if (_mass != 0)
			_collisionShape->calculateLocalInertia(_mass, _inertia);
		_rigidBody = _manager->getObjectPool().create<btRigidBody>(_mass, _motionState, _collisionShape, _inertia);
		_rigidBody->setUserPointer((void*)(entity.getPtr()));
		_rigidBody->setAngularFactor(convertGLMVectorToBullet(_rotationConstraint));
		_rigidBody->setLinearFactor(convertGLMVectorToBullet(_transformConstraint));

		if (_rigidBody->isStaticObject())
		{
			//_rigidBody->setActivationState(DISABLE_SIMULATION);
		}
		_manager->getWorld()->addRigidBody(_rigidBody, filterGroup, filterMask);
		setTransformation(&e.getLink());
	}

	void RigidBody::setRotationConstraint(bool x, bool y, bool z)
	{
		_rotationConstraint = glm::vec3(static_cast<unsigned int>(x),
			static_cast<unsigned int>(y),
			static_cast<unsigned int>(z));
		if (!_rigidBody)
			return;
		_rigidBody->setAngularFactor(convertGLMVectorToBullet(_rotationConstraint));
	}

	void RigidBody::_clearBulletObjects()
	{
		if (_rigidBody != nullptr)
		{
			_manager->getWorld()->removeRigidBody(_rigidBody);
			_manager->getObjectPool().destroy<btRigidBody>(_rigidBody);
		}
		_rigidBody = nullptr;

		if (_motionState != nullptr)
		{
			_manager->getObjectPool().destroy<DynamicMotionState>((DynamicMotionState*)_motionState);
		}
		_motionState = nullptr;

		if (_collisionShape != nullptr)
		{
			if (_shapeType == TypeID::Get<btScaledBvhTriangleMeshShape>())
				_manager->getObjectPool().destroy<btScaledBvhTriangleMeshShape>((btScaledBvhTriangleMeshShape*)_collisionShape);
			else if (_shapeType == TypeID::Get<btConvexHullShape>())
				_manager->getObjectPool().destroy<btConvexHullShape>((btConvexHullShape*)_collisionShape);
			else if (_shapeType == TypeID::Get<btSphereShape>())
				_manager->getObjectPool().destroy<btSphereShape>((btSphereShape*)_collisionShape);
			else if (_shapeType == TypeID::Get<btBoxShape>())
				_manager->getObjectPool().destroy<btBoxShape>((btBoxShape*)_collisionShape);
			else
				assert(false);
		}
		_collisionShape = nullptr;
	}

	void RigidBody::setTransformConstraint(bool x, bool y, bool z)
	{
		_transformConstraint = glm::vec3(static_cast<unsigned int>(x),
			static_cast<unsigned int>(y),
			static_cast<unsigned int>(z));
		if (!_rigidBody)
			return;
		_rigidBody->setLinearFactor(convertGLMVectorToBullet(_transformConstraint));
	}

	RigidBody::~RigidBody(void)
	{
	}

	void RigidBody::postUnserialization(AScene *scene)
	{
		_manager = dynamic_cast<BulletDynamicManager*>(scene->getInstance<BulletCollisionManager>());
	}

#ifdef EDITOR_ENABLED
	void RigidBody::editorCreate(AScene *scene)
	{}

	void RigidBody::editorDelete(AScene *scene)
	{}

	void RigidBody::editorUpdate(AScene *scene)
	{
		//if ((*meshPathList)[selectedMeshIndex] != selectedMeshPath)
		//{
		//	std::size_t i = 0;
		//	for (auto &e : *meshPathList)
		//	{
		//		if (e == selectedMeshPath)
		//		{
		//			selectedMeshIndex = i;
		//			break;
		//		}
		//		++i;
		//	}
		//}
		//if ((*materialPathList)[selectedMaterialIndex] != selectedMaterialPath)
		//{
		//	std::size_t i = 0;
		//	for (auto &e : *materialPathList)
		//	{
		//		if (e == selectedMaterialPath)
		//		{
		//			selectedMaterialIndex = i;
		//			break;
		//		}
		//		++i;
		//	}
		//}

		//ImGui::PushItemWidth(-1);
		//if (ImGui::ListBox("Meshs", (int*)&selectedMeshIndex, &(meshFileList->front()), (int)(meshFileList->size())))
		//{
		//	selectedMeshName = (*meshFileList)[selectedMeshIndex];
		//	selectedMeshPath = (*meshPathList)[selectedMeshIndex];

		//	_mesh = scene->getInstance<AGE::AssetsManager>()->getMesh(selectedMeshPath);

		//	if (!_mesh)
		//	{
		//		scene->getInstance<AGE::AssetsManager>()->loadMesh(OldFile(selectedMeshPath), { AGE::MeshInfos::Positions, AGE::MeshInfos::Normals, AGE::MeshInfos::Uvs, AGE::MeshInfos::Tangents }, selectedMeshPath);

		//		std::size_t totalToLoad = 0;
		//		std::size_t	toLoad = 0;
		//		std::string loadingError;
		//		do {
		//			scene->getInstance<AGE::AssetsManager>()->updateLoadingChannel(selectedMeshPath, totalToLoad, toLoad, loadingError);
		//		} while
		//			(toLoad != 0 && loadingError.size() == 0);
		//	}
		//	_mesh = scene->getInstance<AGE::AssetsManager>()->getMesh(selectedMeshPath);
		//	AGE_ASSERT(_mesh != nullptr);
		//	if (_material)
		//	{
		//		setMeshAndMaterial(_mesh, _material);
		//	}
		//}
		//ImGui::PopItemWidth();
		//ImGui::PushItemWidth(-1);
		//if (!materialFileList->empty() && ImGui::ListBox("Material", (int*)&selectedMaterialIndex, &(materialFileList->front()), (int)(materialFileList->size())))
		//{
		//	selectedMaterialName = (*materialFileList)[selectedMaterialIndex];
		//	selectedMaterialPath = (*materialPathList)[selectedMaterialIndex];

		//	_material = scene->getInstance<AGE::AssetsManager>()->getMaterial(selectedMaterialPath);

		//	if (!_material)
		//	{
		//		scene->getInstance<AGE::AssetsManager>()->loadMaterial(OldFile(selectedMaterialPath), selectedMaterialPath);

		//		std::size_t totalToLoad = 0;
		//		std::size_t	toLoad = 0;
		//		std::string loadingError;
		//		do {
		//			scene->getInstance<AGE::AssetsManager>()->updateLoadingChannel(selectedMaterialPath, totalToLoad, toLoad, loadingError);
		//		} while
		//			(toLoad != 0 && loadingError.size() == 0);
		//	}
		//	_material = scene->getInstance<AGE::AssetsManager>()->getMaterial(selectedMaterialPath);
		//	AGE_ASSERT(_material != nullptr);
		//	if (_mesh)
		//	{
		//		setMeshAndMaterial(_mesh, _material);
		//	}
		//}
		////ImGui::ListBoxFooter();
		//ImGui::PopItemWidth();

	}
#endif
}
