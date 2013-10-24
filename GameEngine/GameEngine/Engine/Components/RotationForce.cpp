#include <glm/gtc/matrix_transform.hpp>
#include "RotationForce.hh"
#include "Core/Engine.hh"

namespace Components
{

	RotationForce::RotationForce(const glm::vec3 &force)
		: AComponent("RotationForce")
		, _force(force)
	{}

	RotationForce::~RotationForce(void)
	{}

	void RotationForce::start()
	{}

	void RotationForce::update()
	{
		glm::vec3 force = _force;
		force *= GameEngine::instance()->timer().getElapsed();
		glm::mat4 mat;

		mat = glm::rotate(mat, force.x, glm::vec3(1,0,0));
		mat = glm::rotate(mat, force.y, glm::vec3(0,1,0));
		mat = glm::rotate(mat, force.z, glm::vec3(0,0,1));
		getFather()->setLocalTransform() = mat;
	}

	void RotationForce::stop()
	{}

	void RotationForce::setForce(const glm::vec3 &force)
	{
		_force = force;
	}

	const glm::vec3 &RotationForce::getForce() const
	{
		return _force;
	}
};