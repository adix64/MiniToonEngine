#pragma once
#include <Component/CharacterAnimation/SkeletalAnimaPosing.h>

glm::vec3 CalcInterpolatedScaling(const SkelTreeAnimTrack *bone, float AnimationTime)
{
	const float inBetweenF = AnimationTime * 24;
	uint ScalingIndex = (uint)glm::floor(inBetweenF);
	uint NextScalingIndex = ScalingIndex + 1;

	if (NextScalingIndex>= bone->m_scalingKeys.size())
		return bone->m_scalingKeys[bone->m_rotationKeys.size() - 1];

	float Factor = inBetweenF - ScalingIndex;
	const glm::vec3 Start = bone->m_scalingKeys[ScalingIndex];
	const glm::vec3 End = bone->m_scalingKeys[NextScalingIndex];
	glm::vec3 Delta = End - Start;
	return Start + Factor * Delta;
}

////////////////////////////////////////////////////////////////////////////////

glm::quat CalcInterpolatedRotation(const SkelTreeAnimTrack *bone, float AnimationTime)
{
	const float inBetweenF = AnimationTime * 24;
	uint rotIdx = (uint)glm::floor(inBetweenF);
	uint nextRotIdx = rotIdx + 1;
#pragma message "THIS IS WRONG!!!!!"
	uint sz = bone->m_rotationKeys.size();
	if (nextRotIdx >= sz) {
		if (sz > 0)
			return bone->m_rotationKeys[bone->m_rotationKeys.size() - 1];
		else
			return glm::quat(0, 0, 1, 0);
	}
	float Factor = inBetweenF - rotIdx;
	const glm::quat startRotQuat = bone->m_rotationKeys[rotIdx];
	const glm::quat endRotQuat = bone->m_rotationKeys[nextRotIdx];
	return glm::normalize(glm::slerp(startRotQuat, endRotQuat, Factor));
}

////////////////////////////////////////////////////////////////////////////////

glm::vec3 CalcInterpolatedPosition(const SkelTreeAnimTrack *bone, float AnimationTime)
{
	const float inBetweenF = AnimationTime * 24;
	uint posIdx = (uint)glm::floor(inBetweenF);
	uint nextPosIdx = posIdx + 1;
#pragma message "THIS IS WRONG!!!!!"
	uint sz = bone->m_translationKeys.size();
	if (nextPosIdx >= sz)
	{
		if (sz > 0)
			return bone->m_translationKeys[bone->m_translationKeys.size() - 1];
		else
			return glm::vec3(0, 0, 0);
	}
	float Factor = inBetweenF - posIdx;
	const glm::vec3 Start = bone->m_translationKeys[posIdx];
	const glm::vec3 End = bone->m_translationKeys[nextPosIdx];
	glm::vec3 Delta = End - Start;
	return Start + Factor * Delta;
}

////////////////////////////////////////////////////////////////////////////////

glm::mat4 convertMatrix4x4(const aiMatrix4x4 m)
{
	glm::mat4 Matri;

	Matri[0][0] = m.a1;
	Matri[0][1] = m.b1;
	Matri[0][2] = m.c1;
	Matri[0][3] = m.d1;
	Matri[1][0] = m.a2;
	Matri[1][1] = m.b2;
	Matri[1][2] = m.c2;
	Matri[1][3] = m.d2;
	Matri[2][0] = m.a3;
	Matri[2][1] = m.b3;
	Matri[2][2] = m.c3;
	Matri[2][3] = m.d3;
	Matri[3][0] = m.a4;
	Matri[3][1] = m.b4;
	Matri[3][2] = m.c4;
	Matri[3][3] = m.d4;

	return Matri;
}

////////////////////////////////////////////////////////////////////////////////

glm::mat4 convertMatrix3x3(const aiMatrix3x3 m)
{
	glm::mat4 Matri = glm::mat4(1);

	Matri[0][0] = m.a1;
	Matri[0][1] = m.b1;
	Matri[0][2] = m.c1;
	Matri[1][0] = m.a2;
	Matri[1][1] = m.b2;
	Matri[1][2] = m.c2;
	Matri[2][0] = m.a3;
	Matri[2][1] = m.b3;
	Matri[2][2] = m.c3;

	return Matri;
}
