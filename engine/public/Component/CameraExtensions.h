#pragma once
#include "../../../dependencies/glm/gtx/rotate_vector.hpp"
#include "utils.hpp" //TODO CLASS CHARACTERACtion
#include <Component/Camera.h>
#include <Rendering/Mesh.h>

/*this object rotates the fps camera and the character with respect to which direction he's WALKING towards*/
class FwdVecOrienter {
public:
	
	glm::vec3 myForward;
	CharacterAction *action;
//pointers to rotatable objects
	int lastActionCode, i;
	Mesh *cameraTarget;
	Camera *camera;
	GLfloat angle, step;
	glm::vec3 rot_axis, last_direction;
	bool hasTarget;
	glm::vec3 target_dir, direction;
	
	FwdVecOrienter(Mesh *cameraTarget, CharacterAction *action, Camera *camera);
	
	void setTargetDir(glm::vec3 td);

	void unsetTargetDir();

	void reset();

//intercepts player input and rotates character with respect to current orientation and current action
	void adjust();

	void adjustWithTarget();

	void adjustWithoutTarget();
};