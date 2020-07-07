#include <Component/Transform.h>

Transform::Transform() : Component()
{
	mName = "Transform";

	mPosition = glm::vec3(0);
	mRotation = glm::quat(0, 0, 0, 0);
	mScale = glm::vec3(1);

	mDirection = glm::vec3(0, 0, 1);
	mRight = glm::vec3(1, 0, 0);
	mUp = glm::vec3(0, 1, 0);

	mLocalMatrix = mWorldMatrix = glm::mat4(1);
	mWV_Matrix = mWVP_Matrix = glm::mat4(NAN);
}
