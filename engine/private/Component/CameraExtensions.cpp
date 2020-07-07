#pragma once
#include <Component/CameraExtensions.h>

/*this object rotates the fps camera and the character with respect to which direction he's WALKING towards*/
void FwdVecOrienter::setTargetDir(glm::vec3 td){
	target_dir = glm::normalize(glm::vec3(td.x, 0, td.z));
	hasTarget = true;
	i = 15;
}
void FwdVecOrienter::unsetTargetDir() {
	hasTarget = false;
	i = 15;
}
FwdVecOrienter::FwdVecOrienter(Mesh *cameraTarget, CharacterAction *action, Camera *camera) {
	hasTarget = false;
	glm::vec3 fwd = camera->getForward();
	myForward = glm::normalize(glm::vec3(fwd.x, 0, fwd.z));
	direction = target_dir =  glm::vec3(0, 0, -1);
	this->action = action;
	this->cameraTarget = cameraTarget;
	this->camera = camera;
	angle = step = 0;
	lastActionCode = 0;
	i = 15;
	last_direction = myForward;
}
void FwdVecOrienter::reset() {
	cameraTarget->SetModelMatrix(glm::translate(glm::mat4(1), glm::vec3(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z)));
	cameraTarget->updateVertices();
	glm::vec3 fwd = camera->getForward();
	myForward = glm::normalize(glm::vec3(fwd.x, 0, fwd.z));
	last_direction = myForward;
	angle = step = 0;
	lastActionCode = 0;
	i = 15;
}
//intercepts player input and rotates character with respect to current orientation and current action
void FwdVecOrienter::adjust()
{
	if (hasTarget) adjustWithTarget();
	else adjustWithoutTarget();
}

void FwdVecOrienter::adjustWithTarget()
{		
	angle = acos(glm::clamp(glm::dot(target_dir, myForward),-1.f,1.f));
	if (fabs(angle) > 0){
		step = angle / 5.f;
		i = 0;
		rot_axis = glm::cross(myForward, target_dir);
		if (rot_axis == glm::vec3(0, 0, 0)) {
			float fact;
			if (rand() % 2) fact = -1;
			else fact = 1;
			rot_axis = glm::vec3(0, fact, 0);
		}
		else rot_axis = glm::normalize(rot_axis);
	}
	if (i < 15) {
		cameraTarget->SetModelMatrix(glm::rotate(cameraTarget->GetModelMatrix(), step, rot_axis));
		myForward = glm::normalize(glm::mat3(glm::rotate(glm::mat4(1), step, rot_axis)) * myForward);
		i++;
	}
}

void FwdVecOrienter::adjustWithoutTarget()
{
	glm::vec2 xzDir = action->movement_dir;
	if (fabs(xzDir.x) > 0.01 || fabs(xzDir.y) > 0.01)
 		xzDir = glm::normalize(xzDir);
	else direction = glm::vec3(0, 0, 0);
//	direction = glm::vec3(1, 0, 0);

	if (direction == glm::vec3(0, 0, 0))
	{
		direction = last_direction;
	}
	else
	{
		glm::vec3 camfwd = camera->getForward(), camright = camera->getRight();
		camfwd.y = 0; camright.y = 0;
		camfwd = glm::normalize(camfwd); camright = glm::normalize(camright);
		direction = camfwd * xzDir.y - camright * xzDir.x;
		direction.y = 0;
		direction = glm::normalize(direction);
	}
	
	if (direction != last_direction ) {
		last_direction = direction;

		angle = acos(glm::clamp(glm::dot(direction, myForward), -1.f, 1.f));
		if (fabs(angle) > 0) {
			step = angle / 15.f;
			i = 0;
			rot_axis = glm::cross(myForward, direction);
			if (rot_axis == glm::vec3(0, 0, 0)) {
				float fact;
				if (rand() % 2) fact = -1;
				else fact = 1;
				rot_axis = glm::vec3(0, fact, 0);
			}
			else rot_axis = glm::normalize(rot_axis);
		}

	}

	if (i < 15) {
		cameraTarget->SetModelMatrix(glm::rotate(cameraTarget->GetModelMatrix(), step, rot_axis));
		myForward = glm::normalize(glm::mat3(glm::rotate(glm::mat4(1), step, rot_axis)) * myForward);
		i++;
	}
}