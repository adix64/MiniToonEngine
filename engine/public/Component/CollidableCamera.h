#pragma once
#include <Component/Camera.h>
#include <Rendering/Mesh.h>
#define camera_position position
class CollidableCamera : public Camera {
//private:
public:
	glm::vec3 target_position;

	std::vector<Mesh*> environment;
	glm::mat4 rotation, scaling;
	float scale_factor;
	bool aicharacter;
//public:
	CollidableCamera();

	CollidableCamera(bool b);

	void Update();

	void disableCollisions(bool b);

	void reset(glm::vec3 center);

	void set(glm::vec3 tpos, glm::vec3 _fwd, glm::vec3 _up);

	void moveForwardKeepOY(float distance);

	void Translate(glm::vec3 d);

	void SetTargetPosition(glm::vec3 pos);
		
	void translateForward(float distance);

	void translateUpword(float distance);

	void translateUpWithWorld(float distance);

	void translateRight(float distance);

	void rotateTPS_OX(float angle);

	void rotateTPS_OY(float angle);

	void rotateTPS_OZ(float angle);

	void setEnvironment(std::vector<Mesh*> &env);

	void placeCamera();

	glm::vec3 getTargetPosition();

	glm::vec3 getPosition();

	glm::vec3 getForward();

	glm::vec3 getRight();
};
