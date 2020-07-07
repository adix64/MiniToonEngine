//////////////////////////////////////////////////////////////////////////////
#include <Component/CharacterAnimation/AttackAnimationState.h>
#include <GlobalTime.h>
#include <ResourceManager.h>

static float ROT_SPEED = 5.f;
AttackAnimationState::AttackAnimationState(){}
AttackAnimationState::AttackAnimationState(uint16_t stateID, int attackFileTag, std::string & chname) : BaseAnimationState(stateID)
{
	ResourceManager &resmgr = ResourceManager::getInstance();
	ResourceManager::CharAnimNFO animNFO = resmgr.GetAnimNFO(chname, attackFileTag);
	m_animName = animNFO.fileName;
	m_Yrotation = 0.f;
	m_animation = resmgr.GetCharacterAnimationTrack(m_animName);
	m_duration = m_animation->GetDuration();
	sweetSpot = animNFO.contact;
}

AttackAnimationState::AttackAnimationState(uint16_t stateID, CharacterAnimation * _anim) : BaseAnimationState(stateID)
{
	sweetSpot = 0.0f;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	m_Yrotation = 0.f;
	m_animation = _anim;
	m_duration = m_animation->GetDuration();
}

float AttackAnimationState::GetRotation(float deltaTrans)
{
	if (fabs(m_Yrotation * 10.f) < 0.001f) return 0;
	//m_Yrotation /= 1 + deltaTrans * 10 / 3;
	return m_Yrotation;
}
void AttackAnimationState::CalcBlendFactors(float wr_speed, float moveDirDotProduct, glm::vec3 &moveDirCrossProduct,
													float enemyDotProduct, glm::vec3 &enemyCrossProduct)
{
	m_Yrotation = glm::acos(glm::clamp(enemyDotProduct, -1.f, 1.f)) * GTHTimes::Time_deltaTime * ROT_SPEED;
	if (fabs(enemyCrossProduct.y) > 0.0001)//fabs(m_enemyDirPtr->x) > 0.01 || fabs(m_enemyDirPtr->z) > 0.01)
	{
		//float enemyDotProduct = glm::dot(glm::normalize(*m_characterFwdVecPtr), glm::normalize(*m_enemyDirPtr));
		//glm::vec3 enemyCrossProduct = glm::cross(glm::normalize(*m_characterFwdVecPtr), glm::normalize(*m_enemyDirPtr));
		m_Yrotation = glm::acos(glm::clamp(enemyDotProduct, -1.f, 1.f)) * GTHTimes::Time_deltaTime * ROT_SPEED;

		if (enemyCrossProduct.y < 0)
		{
			m_Yrotation *= -1;
		}
	}
	else
	{
		m_Yrotation = 0.0f;
	}


}

std::vector<std::pair<float, CharacterAnimation*> > AttackAnimationState::getBlendScheme()
{
	std::vector<std::pair<float, CharacterAnimation*> > blendScheme;
	blendScheme.push_back(std::pair<float, CharacterAnimation*>(1.f, m_animation));
	return blendScheme;
}


//////////////////////////////////////////////////////////////////////

TakeAmortizedHitAnimState::TakeAmortizedHitAnimState() {}
TakeAmortizedHitAnimState::TakeAmortizedHitAnimState(uint16_t stateID, CharacterAnimation *_takeHit, CharacterAnimation *_block) : BaseAnimationState(stateID)
{
	takeHitAnim = _takeHit;
	blockAnim = _block;
	m_Yrotation = 0.f;
	m_duration = takeHitAnim->GetDuration();
}

float TakeAmortizedHitAnimState::GetRotation(float deltaTrans)
{
	if (fabs(m_Yrotation * 10.f) < 0.001f) return 0;
	//m_Yrotation /= 1 + deltaTrans * 10 / 3;
	return m_Yrotation;
}
void TakeAmortizedHitAnimState::CalcBlendFactors(float wr_speed, float moveDirDotProduct, glm::vec3 &moveDirCrossProduct,
	float enemyDotProduct, glm::vec3 &enemyCrossProduct)
{
	m_Yrotation = glm::acos(glm::clamp(enemyDotProduct, -1.f, 1.f)) * GTHTimes::Time_deltaTime * ROT_SPEED;
	if (fabs(enemyCrossProduct.y) > 0.0001)//fabs(m_enemyDirPtr->x) > 0.01 || fabs(m_enemyDirPtr->z) > 0.01)
	{
		//float enemyDotProduct = glm::dot(glm::normalize(*m_characterFwdVecPtr), glm::normalize(*m_enemyDirPtr));
		//glm::vec3 enemyCrossProduct = glm::cross(glm::normalize(*m_characterFwdVecPtr), glm::normalize(*m_enemyDirPtr));
		m_Yrotation = glm::acos(glm::clamp(enemyDotProduct, -1.f, 1.f)) * GTHTimes::Time_deltaTime * ROT_SPEED;

		if (enemyCrossProduct.y < 0)
		{
			m_Yrotation *= -1;
		}
	}
	else
	{
		m_Yrotation = 0.0f;
	}


}

std::vector<std::pair<float, CharacterAnimation*> > TakeAmortizedHitAnimState::getBlendScheme()
{
	std::vector<std::pair<float, CharacterAnimation*> > blendScheme;
	blendScheme.push_back(std::pair<float, CharacterAnimation*>(0.16f, takeHitAnim));
	blendScheme.push_back(std::pair<float, CharacterAnimation*>(0.84f, blockAnim));
	return blendScheme;
}



/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

HitReactAnimState::HitReactAnimState() {}
HitReactAnimState::HitReactAnimState(uint16_t stateID, std::vector<std::string> &animaFilenames) : BaseAnimationState(stateID)
{
	ResourceManager &resmgr = ResourceManager::getInstance();
	m_animName = animaFilenames[0];
	m_Yrotation = 0.f;
	for (int i = 0; i < animaFilenames.size(); i++)
	{
		hitAnims.push_back(resmgr.GetCharacterAnimationTrack(animaFilenames[i]));
	}
	m_duration = hitAnims[0]->GetDuration();
}

float HitReactAnimState::GetRotation(float)
{
	return 0.0f;
}

void HitReactAnimState::CalcBlendFactors(float wr_speed, float moveDirDotProduct, glm::vec3 &moveDirCrossProduct,
	float enemyDotProduct, glm::vec3 &enemyCrossProduct)
{
}


void HitReactAnimState::SetTag(int tag)
{
	this->stateTag = tag;
	this->m_duration = hitAnims[tag]->GetDuration();
}

std::vector<std::pair<float, CharacterAnimation*> > HitReactAnimState::getBlendScheme()
{
	std::vector<std::pair<float, CharacterAnimation*> > blendScheme;
	blendScheme.push_back(std::pair<float, CharacterAnimation*>(1.f, hitAnims[stateTag]));
	return blendScheme;
}