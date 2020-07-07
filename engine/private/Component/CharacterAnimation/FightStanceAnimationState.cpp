#include <Component/CharacterAnimation/FightStanceAnimationState.h>
#include <InputManager.h>
#include <GlobalTime.h>

static float ROT_SPEED = 50.f;
void FightStanceLocomotionAnimationState::SetBlockMode(bool isBlocking)
	{
		m_playSpeed = isBlocking?0.6f:m_playSpeed;
		m_isBlocking = isBlocking;
		m_Yrotation = 0; //!!!!!!!!!!!!
	}

void FightStanceLocomotionAnimationState::CalcBlendFactors(float wr_speed, float moveDirDotProduct, glm::vec3 &moveDirCrossProduct,
	float enemyDotProduct, glm::vec3 &enemyCrossProduct)
{
	if (m_isBlocking)
	{
		m_idleBlendFact = m_move_FwdL_BlendFact = m_move_FwdR_BlendFact =
			m_move_LeftFwd_BlendFact = m_move_RightFwd_BlendFact =
			m_move_Left_BlendFact = m_move_Right_BlendFact =
			m_move_LeftBack_BlendFact = m_move_RightBack_BlendFact =
			m_move_BackL_BlendFact = m_move_BackR_BlendFact = 0.0f;
		
		m_blockBlendFact = 1.0f;
		SavePrevAndBlendWithIt();
		return;
	}

//	float wr_speed = glm::distance(glm::vec2(0, 0), glm::vec2(m_moveDirPtr->x, m_moveDirPtr->z));
	m_idleBlendFact = m_blockBlendFact = m_move_FwdL_BlendFact = m_move_FwdR_BlendFact =
		m_move_LeftFwd_BlendFact = m_move_RightFwd_BlendFact =
		m_move_Left_BlendFact = m_move_Right_BlendFact =
		m_move_LeftBack_BlendFact = m_move_RightBack_BlendFact =
		m_move_BackL_BlendFact = m_move_BackR_BlendFact = 0.0f;

	float speed;

	//NO movement
	if (wr_speed < 0.08f )//fabs(m_moveDirPtr->x) < MIN_JOYSTICK_SENSITIVITY && fabs(m_moveDirPtr->z) < MIN_JOYSTICK_SENSITIVITY)
	{
		//printf(" <2>");
		m_Yrotation = 0;
		m_idleBlendFact = 1;
		m_playSpeed = 0.25f - fabs((float)rand() / RAND_MAX) * 0.05;
		//	//printf("ANGLE OF ROT>>>>>>>>> [%f]\n", m_Yrotation);
		SavePrevAndBlendWithIt();//SavePrev();
		return;
	}

	//float dotProduct = glm::dot(glm::normalize(*m_characterFwdVecPtr), glm::normalize(*m_moveDirPtr));
	//glm::vec3 crossProduct = glm::cross(glm::normalize(*m_characterFwdVecPtr), glm::normalize(*m_moveDirPtr));

	if (fabs(enemyCrossProduct.y) > 0.0001)//fabs(m_enemyDirPtr->x) > 0.01 || fabs(m_enemyDirPtr->z) > 0.01)
	{
		//float enemyDotProduct = glm::dot(glm::normalize(*m_characterFwdVecPtr), glm::normalize(*m_enemyDirPtr));
		//glm::vec3 enemyCrossProduct = glm::cross(glm::normalize(*m_characterFwdVecPtr), glm::normalize(*m_enemyDirPtr));
		m_Yrotation = acos(glm::clamp(enemyDotProduct, -1.f, 1.f));// *GTHTimes::Time_deltaTime * ROT_SPEED;
		//printf(" <3> [%5.2f] ", m_Yrotation);
		if (enemyCrossProduct.y < 0)
		{
			m_Yrotation *= -1;
		}
	}
	else
	{
		//printf(" <1>");
		m_Yrotation = 0.0f;
	}

	{
		speed = glm::clamp(wr_speed, 0.f, 1.f);
		////printf("Velocity:::: %f\n", speed);
		m_playSpeed = speed;

		speed = pow(speed, 0.025);
		//m_playSpeed = speed * speed;// glm::clamp(pow(1 + speed, 3 * (speed + 1)) / 10.f, 0.f, 1.f);
		//m_playSpeed *= m_playSpeed * 1.15;

		//Dir == Fwd
		if (moveDirDotProduct > 0.95)
		{
			m_idleBlendFact = 1 - speed;
			m_move_FwdR_BlendFact = speed;
			SavePrevAndBlendWithIt();//SavePrev();
			return;
		}
		//Dir == -Fwd
		if (moveDirDotProduct < -0.95)
		{
			m_idleBlendFact = 1 - speed;
			m_move_BackL_BlendFact = speed;
			SavePrevAndBlendWithIt();//SavePrev();
			return;
		}

		if (moveDirCrossProduct.y < 0.0f)
		{	//Dir == Fwd
			if (moveDirDotProduct> 0)
			{//RIGHT - FWD
				if (moveDirDotProduct >= 0.5f)
				{// dir : |/
					//printf("FWD-R\n");
					float bf = (moveDirDotProduct - 0.5f) * 2.0f;
					m_move_FwdR_BlendFact = speed * bf;
					m_move_RightFwd_BlendFact = speed * (1 - bf);
					m_idleBlendFact = 1 - speed;
				} 
				else
				{//  dir : /_
					//printf("Right-Fwd\n");
					float bf = moveDirDotProduct * 2.0f;
					m_move_RightFwd_BlendFact = speed * bf;
					m_move_Right_BlendFact = speed * (1 - bf);
					m_idleBlendFact = 1 - speed;
				}
				////printf("RIGHT - FWD\n");
			}
			else
			{//RIGHT - BACK
				if (moveDirDotProduct <= -0.5f)
				{// dir : |\ 
					//printf("Back-R\n");
					float bf = (moveDirDotProduct + 0.5f) * -2.f;
					m_move_BackR_BlendFact = speed * bf;
					m_move_RightBack_BlendFact = speed * (1.f - bf);
					m_idleBlendFact = 1 - speed;
				}
				else 
				{// dir : \-
					//printf("Right-Back\n");
					float bf = moveDirDotProduct * -2.f;
					m_move_RightBack_BlendFact = speed * bf;
					m_move_Right_BlendFact = speed * (1.f - bf);
					m_idleBlendFact = 1 - speed;
				}
				////printf("RIGHT - BACK\n");
			}
		}
		else //LEFT
		{
			//Dir == Fwd
			if (moveDirDotProduct> 0)
			{//LEFT - FWD
				if (moveDirDotProduct >= 0.5f)
				{// dir : \|
					//printf("FWD-L\n");
					float bf = (moveDirDotProduct - 0.5f) * 2.0f;
					m_move_FwdL_BlendFact = speed * bf;
					m_move_LeftFwd_BlendFact = speed * (1 - bf);
					m_idleBlendFact = 1 - speed;
				}
				else
				{//  dir : _\ 
					//printf("Left-Fwd\n");
					float bf = moveDirDotProduct * 2.0f;
					m_move_LeftFwd_BlendFact = speed * bf;
					m_move_Left_BlendFact = speed * (1 - bf);
					m_idleBlendFact = 1 - speed;
				}
				////printf("LEFT - FWD\n");
			}
			else
			{//LEFT - BACK
				if (moveDirDotProduct <= -0.5f)
				{// dir : /|
					//printf("Back-L\n");
					float bf = (moveDirDotProduct + 0.5f) * -2.f;
					m_move_BackL_BlendFact = speed * bf;
					m_move_LeftBack_BlendFact = speed * (1.f - bf);
					m_idleBlendFact = 1 - speed;
				}
				else
				{// dir : -/
					//printf("Left-Back\n");
					float bf = moveDirDotProduct * -2.f;
					m_move_LeftBack_BlendFact = speed * bf;
					m_move_Left_BlendFact = speed * (1.f - bf);
					m_idleBlendFact = 1 - speed;
				}
				////printf("LEFT - BACK\n");
			}
		}
	}

	SavePrevAndBlendWithIt();
}

/////////////////////////////////////////////////////////////////////////////

float FightStanceLocomotionAnimationState::GetRotation(float deltaTrans)
{
	//printf("m_Y_rot = %5.2f\n", m_Yrotation);
	//printf("DeltaTrans %f\n", deltaTrans);

	if (fabs(m_Yrotation) < 0.0001f)
		return 0;	
	return m_Yrotation * 20 * GTHTimes::Time_deltaTime;
	//float ret = m_Yrotation / (1 + deltaTrans);
	//return ret;
}

/////////////////////////////////////////////////////////////////////////////

void FightStanceLocomotionAnimationState::SavePrevAndBlendWithIt()
{
	float bf = glm::clamp(GTHTimes::Time_deltaTime * 8.f, 0.f, 1.f);
	float oneMinusBf = 1.f - bf;
	prev_m_idleBlendFact = m_idleBlendFact = (float)((m_idleBlendFact * bf) + (prev_m_idleBlendFact * oneMinusBf));
	
	prev_m_move_FwdL_BlendFact = m_move_FwdL_BlendFact = (float)((m_move_FwdL_BlendFact * bf) + (prev_m_move_FwdL_BlendFact * oneMinusBf));
	prev_m_move_FwdR_BlendFact = m_move_FwdR_BlendFact = (float)((m_move_FwdR_BlendFact * bf) + (prev_m_move_FwdR_BlendFact * oneMinusBf));

	prev_m_move_RightFwd_BlendFact = m_move_RightFwd_BlendFact = (float)((m_move_RightFwd_BlendFact * bf) + (prev_m_move_RightFwd_BlendFact * oneMinusBf));
	prev_m_move_LeftFwd_BlendFact = m_move_LeftFwd_BlendFact = (float)((m_move_LeftFwd_BlendFact * bf) + (prev_m_move_LeftFwd_BlendFact * oneMinusBf));

	prev_m_move_Left_BlendFact = m_move_Left_BlendFact = (float)((m_move_Left_BlendFact * bf) + (prev_m_move_Left_BlendFact * oneMinusBf));
	prev_m_move_Right_BlendFact = m_move_Right_BlendFact = (float)((m_move_Right_BlendFact * bf) + (prev_m_move_Right_BlendFact * oneMinusBf));

	prev_m_move_LeftBack_BlendFact = m_move_LeftBack_BlendFact = (float)((m_move_LeftBack_BlendFact * bf) + (prev_m_move_LeftBack_BlendFact * oneMinusBf));
	prev_m_move_RightBack_BlendFact = m_move_RightBack_BlendFact = (float)((m_move_RightBack_BlendFact * bf) + (prev_m_move_RightBack_BlendFact * oneMinusBf));

	prev_m_move_BackL_BlendFact = m_move_BackL_BlendFact = (float)((m_move_BackL_BlendFact * bf) + (prev_m_move_BackL_BlendFact * oneMinusBf));
	prev_m_move_BackR_BlendFact = m_move_BackR_BlendFact = (float)((m_move_BackR_BlendFact * bf) + (prev_m_move_BackR_BlendFact * oneMinusBf));
	
	prev_m_blockBlendFact = m_blockBlendFact = (float)((m_blockBlendFact * .5f) + (prev_m_blockBlendFact * 0.5f));
}

/////////////////////////////////////////////////////////////////////////////

void FightStanceLocomotionAnimationState::SavePrev()
{
	prev_m_idleBlendFact = m_idleBlendFact;
	prev_m_move_FwdL_BlendFact = m_move_FwdL_BlendFact;
	prev_m_move_FwdR_BlendFact = m_move_FwdR_BlendFact;

	prev_m_move_RightFwd_BlendFact = m_move_RightFwd_BlendFact;
	prev_m_move_LeftFwd_BlendFact = m_move_LeftFwd_BlendFact;

	prev_m_move_Left_BlendFact = m_move_Left_BlendFact;
	prev_m_move_Right_BlendFact = m_move_Right_BlendFact;

	prev_m_move_LeftBack_BlendFact = m_move_LeftBack_BlendFact;
	prev_m_move_RightBack_BlendFact = m_move_RightBack_BlendFact;

	prev_m_move_BackL_BlendFact = m_move_BackL_BlendFact;
	prev_m_move_BackR_BlendFact = m_move_BackR_BlendFact;

	prev_m_blockBlendFact = m_blockBlendFact;
}

/////////////////////////////////////////////////////////////////////////////

float FightStanceLocomotionAnimationState::GetDuration()
{
#define crtIDLE 0
	return this->m_duration;
	return this->m_idleAnimations[crtIDLE]->m_duration_s;
}

/////////////////////////////////////////////////////////////////////////////
std::vector<std::pair<float, CharacterAnimation*> > FightStanceLocomotionAnimationState::getBlendScheme()
{
	std::vector<std::pair<float, CharacterAnimation*> > scheme;
	//	scheme.reserve(4);//TODO
#define BLEND_EPS 0.00001
	if (fabs(m_idleBlendFact) > BLEND_EPS)
	{
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_idleBlendFact, (CharacterAnimation*)m_idleAnimations[0]));
	}
	if (fabs(m_move_FwdL_BlendFact) > BLEND_EPS)
	{
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_move_FwdL_BlendFact, (CharacterAnimation*)m_move_FwdL_Anim));
	}
	if (fabs(m_move_FwdR_BlendFact) > BLEND_EPS)
	{
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_move_FwdR_BlendFact, (CharacterAnimation*)m_move_FwdR_Anim));
	}
	if (fabs(m_move_LeftFwd_BlendFact) > BLEND_EPS)
	{
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_move_LeftFwd_BlendFact, (CharacterAnimation*)m_move_LeftFwd_Anim));
	}
	if (fabs(m_move_RightFwd_BlendFact) > BLEND_EPS)
	{
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_move_RightFwd_BlendFact, (CharacterAnimation*)m_move_RightFwd_Anim));
	}
	if (fabs(m_move_Left_BlendFact) > BLEND_EPS)
	{
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_move_Left_BlendFact, (CharacterAnimation*)m_move_Left_Anim));
	}

	if (fabs(m_move_Right_BlendFact) > BLEND_EPS)
	{
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_move_Right_BlendFact, (CharacterAnimation*)m_move_Right_Anim));
	}
	if (fabs(m_move_LeftBack_BlendFact) > BLEND_EPS)
	{
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_move_LeftBack_BlendFact, (CharacterAnimation*)m_move_LeftBack_Anim));
	}
	if (fabs(m_move_RightBack_BlendFact) > BLEND_EPS)
	{
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_move_RightBack_BlendFact, (CharacterAnimation*)m_move_RightBack_Anim));
	}
	if (fabs(m_move_BackR_BlendFact) > BLEND_EPS)
	{
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_move_BackR_BlendFact, (CharacterAnimation*)m_move_BackR_Anim));
	}
	if (fabs(m_move_BackL_BlendFact) > BLEND_EPS)
	{
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_move_BackL_BlendFact, (CharacterAnimation*)m_move_BackL_Anim));
	}


	if (fabs(m_blockBlendFact) > BLEND_EPS)
	{
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_blockBlendFact, (CharacterAnimation*)m_blockAnim));
	}

	if (scheme.size() == 0)
	{
		scheme.push_back(std::pair<float, CharacterAnimation*>(1, (CharacterAnimation*)m_idleAnimations[0]));
		int k = -1;
	}
	//#define _PRINTDIRDEBUG
#ifdef _PRINTDIRDEBUG
	//printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	//printf("\t\tBLEND SMOOTHLY:: %f\n", smoothFact);
	//printf("fwd(%f %f %f), dir(%f %f %f)\n)", m_characterFwdVecPtr->x, m_characterFwdVecPtr->y, m_characterFwdVecPtr->z, 
												m_directionVecPtr->x, m_directionVecPtr->y, m_directionVecPtr->z);
	//printf("IDLE blend fact: %f\n", m_idleBlendFact);
	//printf("RUN  FORWARD blend fact: %f\n", m_runFwdBlendFact);
	//printf("RUN  LEFT blend fact: %f\n", m_runLeftBlendFact);
	//printf("RUN  RIGHT blend fact: %f\n\n\n", m_runRightBlendFact);
	//printf("walk FORWARD blend fact: %f\n", m_walkFwdBlendFact);
	//printf("walk LEFT blend fact: %f\n", m_walkLeftBlendFact);
	//printf("walk RIGHT blend fact: %f\n\n\n", m_walkRightBlendFact);
	//	- m_walkFwdBlendFact - m_walkLeftBlendFact - m_walkRightBlendFact;
#endif

	return scheme;
}