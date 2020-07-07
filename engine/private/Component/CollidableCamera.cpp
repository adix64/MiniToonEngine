#pragma once
#include <Component/CollidableCamera.h>
#define INITIAL_DIST_TO_TARGET 7.f
#include <RenderingSystem.h>
//////////////////////////////////////////////////////////////////////////////////

CollidableCamera::CollidableCamera()
{
	target_position = glm::vec3(0, 0, 50);//todo
	forward = glm::vec3(0, 0, -1);
	up = glm::vec3(0, 1, 0);
	right = glm::vec3(1, 0, 0);
	distanceToTarget = 10;
	aicharacter = false;
	viewFrustum.resize(6);
	//	tube->modelMatrix = scaling;
}

//////////////////////////////////////////////////////////////////////////////////

CollidableCamera::CollidableCamera(bool b)
{
	target_position = glm::vec3(0, 0, 50);//todo
	forward = glm::vec3(0, 0, -1);
	up = glm::vec3(0, 1, 0);
	right = glm::vec3(1, 0, 0);
	distanceToTarget = 10;
	aicharacter = b;
	viewFrustum.resize(6);
	//	tube->modelMatrix = scaling;
}

//////////////////////////////////////////////////////////////////////////////////

void CollidableCamera::disableCollisions(bool b){ aicharacter = b; }

//////////////////////////////////////////////////////////////////////////////////

void CollidableCamera::reset(glm::vec3 center)
{
	target_position = center;//todo
	this->forward = glm::vec3(0, 0, -1);
	this->up = glm::vec3(0, 1, 0);
	this->right = glm::vec3(1, 0, 0);
	distanceToTarget = 10;
	camera_position = target_position - forward * distanceToTarget;
	
	if(!aicharacter) placeCamera();
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void CollidableCamera::set(glm::vec3 tpos, glm::vec3 _fwd, glm::vec3 _up)
{
	target_position = tpos;//todo
	this->forward = _fwd;
	this->up = _up;
	this->right = glm::cross(forward,up);
	distanceToTarget = 10;
	camera_position = target_position - forward * distanceToTarget;
	if(!aicharacter) placeCamera();
	updated = false;

}

//////////////////////////////////////////////////////////////////////////////////

void CollidableCamera::moveForwardKeepOY(float distance)
{
	glm::vec3 dir = forward;
	dir.y = 0;
	dir = glm::normalize(dir);
	target_position = target_position + dir * distance;
//	printf("%f\n", dir);
	if(!aicharacter) placeCamera();
	updated = false;

}

//////////////////////////////////////////////////////////////////////////////////

void CollidableCamera::SetTargetPosition(glm::vec3 pos)
{
	target_position = pos;
	if (!aicharacter) placeCamera();
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void CollidableCamera::Translate(glm::vec3 d)
{
	target_position = target_position + d;
	//	printf("%f\n", dir);
	if (!aicharacter) placeCamera();
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void CollidableCamera::translateForward(float distance)
{
	glm::vec3 dir = forward;
	dir = glm::normalize(dir);
	target_position = target_position + dir * distance;
//	printf("%f %f %f\n", dir.x, dir.y, dir.z);
	if(!aicharacter) placeCamera();
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void CollidableCamera::translateUpword(float distance)
{
	glm::vec3 dir = up;
	dir = glm::normalize(dir);
	target_position = target_position + dir * distance;
//	printf("%f %f %f\n", dir.x, dir.y, dir.z);
	if(!aicharacter) placeCamera();
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void CollidableCamera::translateUpWithWorld(float distance)
{
	glm::vec3 dir = glm::vec3(0, 1, 0);
	dir = glm::normalize(dir);
	target_position = target_position + dir * distance;
	if(!aicharacter) placeCamera();
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void CollidableCamera::translateRight(float distance)
{
	glm::vec3 dir = right;
	dir = glm::normalize(dir);
	target_position = target_position + dir * distance;
	if(!aicharacter) placeCamera();
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void CollidableCamera::rotateTPS_OX(float angle)
{
	if (up.y < 0.45f){ 
		return;
	}
	glm::vec4 fwd = glm::vec4(forward, 1);
	fwd = glm::rotate(glm::mat4(1), glm::radians(angle), right) * fwd;
	forward.x = fwd.x; forward.y = fwd.y; forward.z = fwd.z;
	up = glm::cross(right, forward);
	forward = glm::normalize(forward);
	up = glm::normalize(up);
	if (up.y < 0.45f){
		float step = 0.05f;
		if (angle > 0) step = -0.05f;
		do{
			glm::vec4 fwd = glm::vec4(forward, 1);
			fwd = glm::rotate(glm::mat4(1), glm::radians(step), right) * fwd;
			forward.x = fwd.x; forward.y = fwd.y; forward.z = fwd.z;
			up = glm::cross(right, forward);
			forward = glm::normalize(forward);
			up = glm::normalize(up);
		} while (up.y < 0.45f);
	}
	if(!aicharacter) placeCamera();
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void CollidableCamera::rotateTPS_OY(float angle)
{
	forward = RotateOY(forward, glm::radians(angle));
	right = RotateOY(right, glm::radians(angle));
	up = glm::cross(right, forward);
	forward = glm::normalize(forward);
	up = glm::normalize(up);

	if(!aicharacter) placeCamera();
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void CollidableCamera::rotateTPS_OZ(float angle)
{
	glm::vec4 r = glm::vec4(right, 1);
	r = glm::rotate(glm::mat4(1), glm::radians(angle), forward) * r;
	right.x = r.x; right.y = r.y; right.z = r.z;
	up = glm::cross(right, forward);
	right = glm::normalize(right);
	up = glm::normalize(up);

	if(!aicharacter) placeCamera();
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void CollidableCamera::setEnvironment(std::vector<Mesh*> &env)
{
	this->environment = env;
}

//////////////////////////////////////////////////////////////////////////////////

void CollidableCamera::placeCamera()
{
	PhysicsEngine &physicsSys = PhysicsEngine::getInstance();
	glm::vec3 dir = target_position - forward * INITIAL_DIST_TO_TARGET;

	btVector3 End = btVector3(dir.x, dir.y, dir.z);
	btVector3 Start = btVector3(target_position.x, target_position.y, target_position.z);
	btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);
	RayCallback.m_collisionFilterGroup = PhysicsEngine::collisiontypes::COL_RAY;
	RayCallback.m_collisionFilterMask = PhysicsEngine::collisiontypes::COL_WORLD;
	// Perform raycast
	physicsSys.dynamicsWorld->rayTest(Start, End, RayCallback);
	float computedDist = 9999.f;
	if (RayCallback.hasHit()) {
		End = RayCallback.m_hitPointWorld;
		//Normal = RayCallback.m_hitNormalWorld;
		computedDist = glm::distance(glm::vec3(Start.x(), Start.y(), Start.z()), glm::vec3(End.x(), End.y(), End.z()));
		// Do some clever stuff here
	}
	computedDist = std::min(INITIAL_DIST_TO_TARGET, computedDist);
	camera_position = target_position - forward * computedDist;
}

//////////////////////////////////////////////////////////////////////////////////

glm::vec3 CollidableCamera::getTargetPosition()
{
	return target_position;
}

//////////////////////////////////////////////////////////////////////////////////

glm::vec3 CollidableCamera::getPosition()
{
	return camera_position;
}

//////////////////////////////////////////////////////////////////////////////////

glm::vec3 CollidableCamera::getForward()
{
	return forward;
}

//////////////////////////////////////////////////////////////////////////////////

glm::vec3 CollidableCamera::getRight()
{
	return right;
}

void CollidableCamera::Update()
{
	viewMat = glm::lookAt(camera_position, target_position, up);
	projViewMat = projMat * viewMat;
	ComputeViewFrustum();
	updateRevision++;
	updated = true;
}