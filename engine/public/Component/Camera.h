#pragma once
#define _USE_MATH_DEFINES
#include <math.h>


#include "../../../dependencies/glm/glm.hpp"
#include "../../../dependencies/glm/gtc/type_ptr.hpp"
#include "../../../dependencies/glm/gtc/matrix_transform.hpp"

#include <vector>

inline glm::vec3 RotateOY(const glm::vec3 Point, float radians)
{
	glm::vec3 R;
	R.x = Point.x * cos(radians) - Point.z * sin(radians);
	R.y = Point.y;
	R.z = Point.x * sin(radians) + Point.z * cos(radians);
	return R;
}
bool BoxInFrustum(std::vector<glm::vec3> &aabb, std::vector<glm::vec4> &fru);
bool HullInFrustum(std::vector<glm::vec3> &verts, glm::mat4 &pvMat);
bool SphereInFrustum(glm::vec3 &center, float radius, glm::mat4 &pvMat);

class Camera
{
public:
	Camera();

	Camera(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up);

	~Camera();

	// Update camera
	virtual void set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up);

	virtual void moveInDir(float distance, glm::vec3 d);

	virtual void moveForwardKeepOY(float distance);

	virtual void translateForward(float distance);

	virtual void translateUpword(float distance);

	virtual void translateUpWithWorld(float distance);

	virtual void translateRight(float distance);

	virtual void Translate(glm::vec3 d);

	// Rotate the camera in FPS mode over the local OX axis
	virtual void rotateFPS_OX(float angle);

	virtual void rotateFPS_OY(float angle);

	virtual void rotateFPS_OZ(float angle);

	virtual void rotateTPS_OX(float angle);

	virtual void rotateTPS_OY(float angle);

	virtual void rotateTPS_OZ(float angle);

	virtual glm::vec3 getTargetPosition();

	virtual glm::vec3 getPosition();

	virtual glm::vec3 getForward();

	virtual glm::vec3 getRight();
		
	virtual void Update();

	bool AABBinViewFrustum();
		
	void SetOmniShadowMapProjection();
	void SetSpotShadowMapProjection(float cutoffCosine);

	glm::mat4 getViewMatrix();

	glm::mat4 getProjectionMatrix();

	glm::mat4 getProjViewMatrix();

	std::vector<glm::vec4> getViewFrustum();

	void ComputeViewFrustum();
//private:
	float distanceToTarget;
	glm::vec3 position;
	glm::vec3 forward;
	glm::vec3 right;
	glm::vec3 up;
	glm::mat4 projMat, viewMat, projViewMat;
	bool updated = false;
	std::vector<glm::vec4> viewFrustum;
	long long updateRevision = LLONG_MIN;
};


