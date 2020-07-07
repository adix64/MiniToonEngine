#pragma once
#include <Component/Component.h>
#include "../../dependencies/glm/glm.hpp"
#include "../../dependencies/glm/gtc/quaternion.hpp"

class Transform : public Component
{
private:
	glm::vec3 mPosition;
	glm::quat mRotation;
	glm::vec3 mScale;

	glm::vec3 mDirection;
	glm::vec3 mUp, mRight;

	glm::mat4 mLocalMatrix;
	glm::mat4 mWorldMatrix;
	glm::mat4 mWV_Matrix;
	glm::mat4 mWVP_Matrix;

	bool mUpdated = false;
public:
	Transform();
	~Transform() {}

	void SetPosition(glm::vec3 &pPosition) { mPosition = pPosition; mUpdated = false; }
	const glm::vec3& GetPosition() const { return mPosition; }

	void SetRotation(glm::quat &pRotation) { mRotation = pRotation; mUpdated = false; }
	const glm::quat& GetRotation() const { return mRotation; }
	
	void SetScale(glm::vec3 &pScale) { mScale = pScale; mUpdated = false; }
	const glm::vec3& GetScale() const { return mScale; }

	const glm::mat4& GetWorldMatrix() const { return mWorldMatrix; }
	const glm::mat4& GetLocalMatrix() const { return mLocalMatrix; }
	const glm::mat4& GetWVMatrix() const { return mWV_Matrix; }
	const glm::mat4& GetWVPMatrix() const { return mWVP_Matrix; }

	const glm::vec3& GetDirection() const { return mDirection; }
	const glm::vec3& GetRight() const { return mRight; }
	const glm::vec3& GetUp() const { return mUp; }
	
	//!!!
	void SetWorldMatrix(const glm::mat4 &pWorldMatrix) { mWorldMatrix = pWorldMatrix; }
	//void Awake();
	//void Start();
	//void Destroy();

	//void FixedUpdate();
	//void Update();
	//void LateUpdate();
	friend class SceneGraph;
};