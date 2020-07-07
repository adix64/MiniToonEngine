#pragma once
#include <Component/Camera.h>

#define PI 3.1415926f
Camera::Camera()
{
	position = glm::vec3(0, 0, 50);
	forward = glm::vec3(0, 0, -1);
	up		= glm::vec3(0, 1, 0);
	right	= glm::vec3(1, 0, 0);
	distanceToTarget = 9;
	projMat = glm::perspective(PI / 3.f, 16.f / 9.f, 1.f, 10000.f);
	viewFrustum.resize(6);
}

//////////////////////////////////////////////////////////////////////////////////

Camera::Camera(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
{
	viewFrustum.resize(6);
	projMat = glm::perspective(PI / 3.f, 16.f/9.f, 1.f, 10000.f);
	set(position, center,up);
}

//////////////////////////////////////////////////////////////////////////////////

Camera::~Camera()
{ }

//////////////////////////////////////////////////////////////////////////////////

void Camera::SetSpotShadowMapProjection(float cutoffCosine)
{
	projMat = glm::perspective(acos(cutoffCosine) * 2.f, 1.f, 1.f, 500.f);

}
//////////////////////////////////////////////////////////////////////////////////

void Camera::SetOmniShadowMapProjection()
{
	projMat = glm::perspective(PI/2, 1.f, 1.f, 500.f);

}

//////////////////////////////////////////////////////////////////////////////////

void Camera::set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
{
	this->position = position;
	forward = glm::normalize(center-position);
	right	= glm::cross(forward, up);
	this->up = glm::cross(right,forward);
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void Camera::moveInDir(float distance, glm::vec3 d)
{
	glm::vec3 dir = glm::normalize(glm::vec3(d.x, 0, d.z));
	position = position + dir * distance;
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void Camera::Translate(glm::vec3 d)
{
	position = position + d;
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void Camera::moveForwardKeepOY(float distance)
{
	glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
	position = position + dir * distance;
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void Camera::translateForward(float distance)
{
	glm::vec3 dir = glm::normalize(forward);
	position = position + dir * distance;
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void Camera::translateUpword(float distance)
{
	glm::vec3 dir = glm::normalize(up);
	position = position + dir * distance;
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void Camera::translateUpWithWorld(float distance)
{
	glm::vec3 dir = glm::normalize(glm::vec3(0, up.y, 0));
	position = position + dir * distance;
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void Camera::translateRight(float distance)
{
	glm::vec3 dir = glm::normalize(glm::vec3(right.x, 0, right.z));
	position = position + dir * distance;
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void Camera::rotateFPS_OX(float angle)
{
	glm::vec4 fwd = glm::vec4(forward, 1);
	fwd = glm::rotate(glm::mat4(1), glm::radians(angle), right) * fwd;
	forward.x = fwd.x; forward.y = fwd.y; forward.z = fwd.z;
	up = glm::cross(right, forward);
	forward = glm::normalize(forward);
	up = glm::normalize(up);
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void Camera::rotateFPS_OY(float angle)
{
	forward = RotateOY(forward,glm::radians(angle));
	right = RotateOY(right, glm::radians(angle));
	up = glm::cross(right, forward);
	forward = glm::normalize(forward);
	up = glm::normalize(up);
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void Camera::rotateFPS_OZ(float angle)
{
	glm::vec4 r = glm::vec4(right, 1);
	r = glm::rotate(glm::mat4(1), glm::radians(angle), forward) * r;
	right.x = r.x; right.y = r.y; right.z = r.z;
	up = glm::cross(right, forward);
	right = glm::normalize(right);
	up = glm::normalize(up);
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void Camera::rotateTPS_OX(float angle)
{
	translateForward(distanceToTarget);
	rotateFPS_OX(angle);
	translateForward(-distanceToTarget);
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void Camera::rotateTPS_OY(float angle)
{
	translateForward(distanceToTarget);
	rotateFPS_OY(angle);
	translateForward(-distanceToTarget);
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void Camera::rotateTPS_OZ(float angle)
{
	translateForward(distanceToTarget);
	rotateFPS_OZ(angle);
	translateForward(-distanceToTarget);
	updated = false;
}

//////////////////////////////////////////////////////////////////////////////////

void Camera::Update()
{
	viewMat = glm::lookAt(position, position + forward, up);
	projViewMat = projMat * viewMat;
	ComputeViewFrustum();
	updated = true;
	updateRevision++;
}

//////////////////////////////////////////////////////////////////////////////////

glm::mat4 Camera::getViewMatrix()
{
	if (!updated)
		Update();
	return viewMat;
}

//////////////////////////////////////////////////////////////////////////////////

glm::mat4 Camera::getProjViewMatrix()
{
	if (!updated)
		Update();
	return projViewMat;
}

//////////////////////////////////////////////////////////////////////////////////

glm::mat4 Camera::getProjectionMatrix()
{
	if (!updated)
		Update();
	return projMat;
}

//////////////////////////////////////////////////////////////////////////////////

glm::vec3 Camera::getTargetPosition()
{
	return position + forward * distanceToTarget;
}

//////////////////////////////////////////////////////////////////////////////////

glm::vec3 Camera::getPosition()
{
	return position;
}

//////////////////////////////////////////////////////////////////////////////////

glm::vec3 Camera::getForward()
{
	return forward;
}

//////////////////////////////////////////////////////////////////////////////////

glm::vec3 Camera::getRight()
{
	return right;
}

void Camera::ComputeViewFrustum()
{
	float t;
	/* Extract the numbers for the RIGHT plane */
	viewFrustum[0][0] = projViewMat[0][3] - projViewMat[0][0];
	viewFrustum[0][1] = projViewMat[1][3] - projViewMat[1][0];
	viewFrustum[0][2] = projViewMat[2][3] - projViewMat[2][0];
	viewFrustum[0][3] = projViewMat[3][3] - projViewMat[3][0];

	/* Normalize the result */
	t = sqrt(viewFrustum[0][0] * viewFrustum[0][0] + viewFrustum[0][1] * viewFrustum[0][1] + viewFrustum[0][2] * viewFrustum[0][2]);
	viewFrustum[0][0] /= t;
	viewFrustum[0][1] /= t;
	viewFrustum[0][2] /= t;
	viewFrustum[0][3] /= t;

	/* Extract the numbers for the LEFT plane */
	viewFrustum[1][0] = projViewMat[0][3] + projViewMat[0][0];
	viewFrustum[1][1] = projViewMat[1][3] + projViewMat[1][0];
	viewFrustum[1][2] = projViewMat[2][3] + projViewMat[2][0];
	viewFrustum[1][3] = projViewMat[3][3] + projViewMat[3][0];

	/* Normalize the result */
	t = sqrt(viewFrustum[1][0] * viewFrustum[1][0] + viewFrustum[1][1] * viewFrustum[1][1] + viewFrustum[1][2] * viewFrustum[1][2]);
	viewFrustum[1][0] /= t;
	viewFrustum[1][1] /= t;
	viewFrustum[1][2] /= t;
	viewFrustum[1][3] /= t;

	/* Extract the BOTTOM plane */
	viewFrustum[2][0] = projViewMat[0][3] + projViewMat[0][1];
	viewFrustum[2][1] = projViewMat[1][3] + projViewMat[1][1];
	viewFrustum[2][2] = projViewMat[2][3] + projViewMat[2][1];
	viewFrustum[2][3] = projViewMat[3][3] + projViewMat[3][1];

	/* Normalize the result */
	t = sqrt(viewFrustum[2][0] * viewFrustum[2][0] + viewFrustum[2][1] * viewFrustum[2][1] + viewFrustum[2][2] * viewFrustum[2][2]);
	viewFrustum[2][0] /= t;
	viewFrustum[2][1] /= t;
	viewFrustum[2][2] /= t;
	viewFrustum[2][3] /= t;

	/* Extract the TOP plane */
	viewFrustum[3][0] = projViewMat[0][3] - projViewMat[0][1];
	viewFrustum[3][1] = projViewMat[1][3] - projViewMat[1][1];
	viewFrustum[3][2] = projViewMat[2][3] - projViewMat[2][1];
	viewFrustum[3][3] = projViewMat[3][3] - projViewMat[3][1];

	/* Normalize the result */
	t = sqrt(viewFrustum[3][0] * viewFrustum[3][0] + viewFrustum[3][1] * viewFrustum[3][1] + viewFrustum[3][2] * viewFrustum[3][2]);
	viewFrustum[3][0] /= t;
	viewFrustum[3][1] /= t;
	viewFrustum[3][2] /= t;
	viewFrustum[3][3] /= t;

	/* Extract the FAR plane */
	viewFrustum[4][0] = projViewMat[0][3] - projViewMat[0][2];
	viewFrustum[4][1] = projViewMat[1][3] - projViewMat[1][2];
	viewFrustum[4][2] = projViewMat[2][3] - projViewMat[2][2];
	viewFrustum[4][3] = projViewMat[3][3] - projViewMat[3][2];

	/* Normalize the result */
	t = sqrt(viewFrustum[4][0] * viewFrustum[4][0] + viewFrustum[4][1] * viewFrustum[4][1] + viewFrustum[4][2] * viewFrustum[4][2]);
	viewFrustum[4][0] /= t;
	viewFrustum[4][1] /= t;
	viewFrustum[4][2] /= t;
	viewFrustum[4][3] /= t;

	/* Extract the NEAR plane */
	viewFrustum[5][0] = projViewMat[0][3] + projViewMat[0][2];
	viewFrustum[5][1] = projViewMat[1][3] + projViewMat[1][2];
	viewFrustum[5][2] = projViewMat[2][3] + projViewMat[2][2];
	viewFrustum[5][3] = projViewMat[3][3] + projViewMat[3][2];

	/* Normalize the result */
	t = sqrt(viewFrustum[5][0] * viewFrustum[5][0] + viewFrustum[5][1] * viewFrustum[5][1] + viewFrustum[5][2] * viewFrustum[5][2]);
	viewFrustum[5][0] /= t;
	viewFrustum[5][1] /= t;
	viewFrustum[5][2] /= t;
	viewFrustum[5][3] /= t;
}

std::vector<glm::vec4> Camera::getViewFrustum()
{
	if (!updated)
		Update();
	return viewFrustum;
}

bool BoxInFrustum(std::vector<glm::vec3> &aabb, std::vector<glm::vec4> &fru)
{
	//return true;
	for (int i = 0; i < 6; i++)
	{
		int out = 0;
		out += ((glm::dot(fru[i], glm::vec4(aabb[0], 1.0f)) < 0.0) ? 1 : 0);
		out += ((glm::dot(fru[i], glm::vec4(aabb[1], 1.0f)) < 0.0) ? 1 : 0);
		out += ((glm::dot(fru[i], glm::vec4(aabb[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((glm::dot(fru[i], glm::vec4(aabb[3], 1.0f)) < 0.0) ? 1 : 0);
		out += ((glm::dot(fru[i], glm::vec4(aabb[4], 1.0f)) < 0.0) ? 1 : 0);
		out += ((glm::dot(fru[i], glm::vec4(aabb[5], 1.0f)) < 0.0) ? 1 : 0);
		out += ((glm::dot(fru[i], glm::vec4(aabb[6], 1.0f)) < 0.0) ? 1 : 0);
		out += ((glm::dot(fru[i], glm::vec4(aabb[7], 1.0f)) < 0.0) ? 1 : 0);
		if (out == 8) return false;
	}

	//// check frustum outside/inside box
	//int out;
	//out = 0; for (int i = 0; i<8; i++) out += ((fru.mPoints[i].x > box.mMaxX) ? 1 : 0); if (out == 8) return false;
	//out = 0; for (int i = 0; i<8; i++) out += ((fru.mPoints[i].x < box.mMinX) ? 1 : 0); if (out == 8) return false;
	//out = 0; for (int i = 0; i<8; i++) out += ((fru.mPoints[i].y > box.mMaxY) ? 1 : 0); if (out == 8) return false;
	//out = 0; for (int i = 0; i<8; i++) out += ((fru.mPoints[i].y < box.mMinY) ? 1 : 0); if (out == 8) return false;
	//out = 0; for (int i = 0; i<8; i++) out += ((fru.mPoints[i].z > box.mMaxZ) ? 1 : 0); if (out == 8) return false;
	//out = 0; for (int i = 0; i<8; i++) out += ((fru.mPoints[i].z < box.mMinZ) ? 1 : 0); if (out == 8) return false;

	return true;
}

bool HullInFrustum(std::vector<glm::vec3> &verts, glm::mat4 &pvMat)
{
	std::vector<glm::vec4> viewFrustum;
	viewFrustum.resize(6);
	float t;
	/* Extract the numbers for the RIGHT plane */
	viewFrustum[0][0] = pvMat[0][3] - pvMat[0][0];
	viewFrustum[0][1] = pvMat[1][3] - pvMat[1][0];
	viewFrustum[0][2] = pvMat[2][3] - pvMat[2][0];
	viewFrustum[0][3] = pvMat[3][3] - pvMat[3][0];

	/* Normalize the result */
	t = sqrt(viewFrustum[0][0] * viewFrustum[0][0] + viewFrustum[0][1] * viewFrustum[0][1] + viewFrustum[0][2] * viewFrustum[0][2]);
	viewFrustum[0][0] /= t;
	viewFrustum[0][1] /= t;
	viewFrustum[0][2] /= t;
	viewFrustum[0][3] /= t;

	/* Extract the numbers for the LEFT plane */
	viewFrustum[1][0] = pvMat[0][3] + pvMat[0][0];
	viewFrustum[1][1] = pvMat[1][3] + pvMat[1][0];
	viewFrustum[1][2] = pvMat[2][3] + pvMat[2][0];
	viewFrustum[1][3] = pvMat[3][3] + pvMat[3][0];

	/* Normalize the result */
	t = sqrt(viewFrustum[1][0] * viewFrustum[1][0] + viewFrustum[1][1] * viewFrustum[1][1] + viewFrustum[1][2] * viewFrustum[1][2]);
	viewFrustum[1][0] /= t;
	viewFrustum[1][1] /= t;
	viewFrustum[1][2] /= t;
	viewFrustum[1][3] /= t;

	/* Extract the BOTTOM plane */
	viewFrustum[2][0] = pvMat[0][3] + pvMat[0][1];
	viewFrustum[2][1] = pvMat[1][3] + pvMat[1][1];
	viewFrustum[2][2] = pvMat[2][3] + pvMat[2][1];
	viewFrustum[2][3] = pvMat[3][3] + pvMat[3][1];

	/* Normalize the result */
	t = sqrt(viewFrustum[2][0] * viewFrustum[2][0] + viewFrustum[2][1] * viewFrustum[2][1] + viewFrustum[2][2] * viewFrustum[2][2]);
	viewFrustum[2][0] /= t;
	viewFrustum[2][1] /= t;
	viewFrustum[2][2] /= t;
	viewFrustum[2][3] /= t;

	/* Extract the TOP plane */
	viewFrustum[3][0] = pvMat[0][3] - pvMat[0][1];
	viewFrustum[3][1] = pvMat[1][3] - pvMat[1][1];
	viewFrustum[3][2] = pvMat[2][3] - pvMat[2][1];
	viewFrustum[3][3] = pvMat[3][3] - pvMat[3][1];

	/* Normalize the result */
	t = sqrt(viewFrustum[3][0] * viewFrustum[3][0] + viewFrustum[3][1] * viewFrustum[3][1] + viewFrustum[3][2] * viewFrustum[3][2]);
	viewFrustum[3][0] /= t;
	viewFrustum[3][1] /= t;
	viewFrustum[3][2] /= t;
	viewFrustum[3][3] /= t;

	/* Extract the FAR plane */
	viewFrustum[4][0] = pvMat[0][3] - pvMat[0][2];
	viewFrustum[4][1] = pvMat[1][3] - pvMat[1][2];
	viewFrustum[4][2] = pvMat[2][3] - pvMat[2][2];
	viewFrustum[4][3] = pvMat[3][3] - pvMat[3][2];

	/* Normalize the result */
	t = sqrt(viewFrustum[4][0] * viewFrustum[4][0] + viewFrustum[4][1] * viewFrustum[4][1] + viewFrustum[4][2] * viewFrustum[4][2]);
	viewFrustum[4][0] /= t;
	viewFrustum[4][1] /= t;
	viewFrustum[4][2] /= t;
	viewFrustum[4][3] /= t;

	/* Extract the NEAR plane */
	viewFrustum[5][0] = pvMat[0][3] + pvMat[0][2];
	viewFrustum[5][1] = pvMat[1][3] + pvMat[1][2];
	viewFrustum[5][2] = pvMat[2][3] + pvMat[2][2];
	viewFrustum[5][3] = pvMat[3][3] + pvMat[3][2];

	/* Normalize the result */
	t = sqrt(viewFrustum[5][0] * viewFrustum[5][0] + viewFrustum[5][1] * viewFrustum[5][1] + viewFrustum[5][2] * viewFrustum[5][2]);
	viewFrustum[5][0] /= t;
	viewFrustum[5][1] /= t;
	viewFrustum[5][2] /= t;
	viewFrustum[5][3] /= t;

	//return true;
	for (int i = 0; i < 6; i++)
	{
		int out = 0;
		size_t sz = verts.size();
		
		for(int v = 0; v < sz; v++)
			out += ((glm::dot(viewFrustum[i], glm::vec4(verts[v], 1.0f)) < 0.0) ? 1 : 0);
		
		if (out == sz) 
			return false;
	}
	return true;
}


bool SphereInFrustum(glm::vec3 &center, float radius, glm::mat4 &pvMat)
{
	std::vector<glm::vec4> viewFrustum;
	viewFrustum.resize(6);
	float t;
	/* Extract the numbers for the RIGHT plane */
	viewFrustum[0][0] = pvMat[0][3] - pvMat[0][0];
	viewFrustum[0][1] = pvMat[1][3] - pvMat[1][0];
	viewFrustum[0][2] = pvMat[2][3] - pvMat[2][0];
	viewFrustum[0][3] = pvMat[3][3] - pvMat[3][0];

	/* Normalize the result */
	t = sqrt(viewFrustum[0][0] * viewFrustum[0][0] + viewFrustum[0][1] * viewFrustum[0][1] + viewFrustum[0][2] * viewFrustum[0][2]);
	viewFrustum[0][0] /= t;
	viewFrustum[0][1] /= t;
	viewFrustum[0][2] /= t;
	viewFrustum[0][3] /= t;

	/* Extract the numbers for the LEFT plane */
	viewFrustum[1][0] = pvMat[0][3] + pvMat[0][0];
	viewFrustum[1][1] = pvMat[1][3] + pvMat[1][0];
	viewFrustum[1][2] = pvMat[2][3] + pvMat[2][0];
	viewFrustum[1][3] = pvMat[3][3] + pvMat[3][0];

	/* Normalize the result */
	t = sqrt(viewFrustum[1][0] * viewFrustum[1][0] + viewFrustum[1][1] * viewFrustum[1][1] + viewFrustum[1][2] * viewFrustum[1][2]);
	viewFrustum[1][0] /= t;
	viewFrustum[1][1] /= t;
	viewFrustum[1][2] /= t;
	viewFrustum[1][3] /= t;

	/* Extract the BOTTOM plane */
	viewFrustum[2][0] = pvMat[0][3] + pvMat[0][1];
	viewFrustum[2][1] = pvMat[1][3] + pvMat[1][1];
	viewFrustum[2][2] = pvMat[2][3] + pvMat[2][1];
	viewFrustum[2][3] = pvMat[3][3] + pvMat[3][1];

	/* Normalize the result */
	t = sqrt(viewFrustum[2][0] * viewFrustum[2][0] + viewFrustum[2][1] * viewFrustum[2][1] + viewFrustum[2][2] * viewFrustum[2][2]);
	viewFrustum[2][0] /= t;
	viewFrustum[2][1] /= t;
	viewFrustum[2][2] /= t;
	viewFrustum[2][3] /= t;

	/* Extract the TOP plane */
	viewFrustum[3][0] = pvMat[0][3] - pvMat[0][1];
	viewFrustum[3][1] = pvMat[1][3] - pvMat[1][1];
	viewFrustum[3][2] = pvMat[2][3] - pvMat[2][1];
	viewFrustum[3][3] = pvMat[3][3] - pvMat[3][1];

	/* Normalize the result */
	t = sqrt(viewFrustum[3][0] * viewFrustum[3][0] + viewFrustum[3][1] * viewFrustum[3][1] + viewFrustum[3][2] * viewFrustum[3][2]);
	viewFrustum[3][0] /= t;
	viewFrustum[3][1] /= t;
	viewFrustum[3][2] /= t;
	viewFrustum[3][3] /= t;

	/* Extract the FAR plane */
	viewFrustum[4][0] = pvMat[0][3] - pvMat[0][2];
	viewFrustum[4][1] = pvMat[1][3] - pvMat[1][2];
	viewFrustum[4][2] = pvMat[2][3] - pvMat[2][2];
	viewFrustum[4][3] = pvMat[3][3] - pvMat[3][2];

	/* Normalize the result */
	t = sqrt(viewFrustum[4][0] * viewFrustum[4][0] + viewFrustum[4][1] * viewFrustum[4][1] + viewFrustum[4][2] * viewFrustum[4][2]);
	viewFrustum[4][0] /= t;
	viewFrustum[4][1] /= t;
	viewFrustum[4][2] /= t;
	viewFrustum[4][3] /= t;

	/* Extract the NEAR plane */
	viewFrustum[5][0] = pvMat[0][3] + pvMat[0][2];
	viewFrustum[5][1] = pvMat[1][3] + pvMat[1][2];
	viewFrustum[5][2] = pvMat[2][3] + pvMat[2][2];
	viewFrustum[5][3] = pvMat[3][3] + pvMat[3][2];

	/* Normalize the result */
	t = sqrt(viewFrustum[5][0] * viewFrustum[5][0] + viewFrustum[5][1] * viewFrustum[5][1] + viewFrustum[5][2] * viewFrustum[5][2]);
	viewFrustum[5][0] /= t;
	viewFrustum[5][1] /= t;
	viewFrustum[5][2] /= t;
	viewFrustum[5][3] /= t;

	//return true;
	for (int i = 0; i < 6; i++)
	{
		if ((glm::dot(viewFrustum[i], glm::vec4(glm::normalize(glm::vec3(viewFrustum[i])) * radius + center, 1.0f)) < 0.0) ? 1 : 0)
			return false;
	}
	return true;
}
