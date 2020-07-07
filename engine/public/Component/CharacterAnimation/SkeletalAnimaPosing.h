#pragma once
#include "../../../dependencies/glm/gtx/quaternion.hpp"
#include "../../../dependencies/glm/glm.hpp"
#include "../../../dependencies/glm/gtx/quaternion.hpp"
#include "../../../dependencies/glm/gtx/dual_quaternion.hpp"
#include "../../../dependencies/glm/gtc/type_ptr.hpp"
#include "../../../dependencies/glm/gtc/matrix_transform.hpp"
#include "../assimp-3.3.1/include/assimp/Importer.hpp"
#include "../assimp-3.3.1/include/assimp/scene.h"
#include "../assimp-3.3.1/include/assimp/postprocess.h"
#include <vector>
#include <ctype.h>

typedef glm::mat2x4 DualQuaternion;
typedef unsigned int uint;


struct SkelTreeAnimTrack
{
	//std::string m_name;
	uint16_t m_boneIdx;
	std::vector<glm::vec3> m_translationKeys;
	std::vector<glm::quat> m_rotationKeys;
	std::vector<glm::vec3> m_scalingKeys;
	std::vector<SkelTreeAnimTrack*> m_children;
	//glm::mat4 m_staticTransf;

	SkelTreeAnimTrack() /*: m_staticTransf(glm::mat4(1))*/{}
	~SkelTreeAnimTrack()
	{
		for (int i = 0; i < m_children.size(); i++)
		{
	//		if (m_children[i])
		//		delete m_children[i];
		}
	}
};

////////////////////////////////////////////////////////////////////////////////
/*
inline glm::quat InterpolateRotation(glm::quat& q1, glm::quat& q2, float &Factor);

inline glm::vec3 InterpolateTranslation(glm::vec3 &v1, glm::vec3 &v2, float &Factor);

inline glm::vec3 InterpolateScaling(glm::vec3 &v1, glm::vec3 &v2, float &Factor);
*/
inline glm::quat InterpolateRotation(glm::quat& q1, glm::quat& q2, float &Factor)
{
	return glm::slerp(q1, q2, Factor);
}

////////////////////////////////////////////////////////////////////////////////

inline glm::vec3 InterpolateTranslation(glm::vec3 &v1, glm::vec3 &v2, float &Factor)
{
	return (v1 + Factor * (v2 - v1));
}

inline glm::vec3 InterpolateScaling(glm::vec3 &v1, glm::vec3 &v2, float &Factor)
{
	return (v1 + Factor * (v2 - v1));
}

glm::vec3 CalcInterpolatedScaling(const SkelTreeAnimTrack *bone, float AnimationTime);

glm::quat CalcInterpolatedRotation(const SkelTreeAnimTrack *bone, float AnimationTime);

glm::vec3 CalcInterpolatedPosition(const SkelTreeAnimTrack *bone, float AnimationTime);

glm::mat4 convertMatrix4x4(const aiMatrix4x4 m);

glm::mat4 convertMatrix3x3(const aiMatrix3x3 m);



