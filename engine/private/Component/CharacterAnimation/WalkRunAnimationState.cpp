#include <Component/CharacterAnimation/WalkRunAnimationState.h>
#include <InputManager.h>
#include <GlobalTime.h>
#include <cmath>
	////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////


static float ROT_SPEED = 15.f;

void WalkRunAnimationState::CalcBlendFactors(float wr_speed, float moveDirDotProduct, glm::vec3 &moveDirCrossProduct,
	float enemyDotProduct, glm::vec3 &enemyCrossProduct)
{
	
	//if (wr_speed != wr_speed)
//	float wr_speed = glm::distance(glm::vec2(0, 0), glm::vec2(m_directionVecPtr->x, m_directionVecPtr->z));
	m_idleBlendFact = m_runFwdBlendFact = m_walkFwdBlendFact =
		m_runLeftBlendFact = m_runRightBlendFact =
		m_walkLeftBlendFact = m_walkRightBlendFact = 0;
#define ANIM_SEPARATOR 0.5f
	float speed;
		

	//NO movement
	if ( wr_speed < 0.08)//fabs(m_directionVecPtr->x) < MIN_JOYSTICK_SENSITIVITY && fabs(m_directionVecPtr->z) < MIN_JOYSTICK_SENSITIVITY)
	{
		m_Yrotation = 0;
		m_idleBlendFact = 1;
		m_playSpeed = 0.25f - fabs((float)rand()/RAND_MAX) * 0.05;
	//	printf("ANGLE OF ROT>>>>>>>>> [%f]\n", m_Yrotation);
		SavePrevAndBlendWithIt();//SavePrev();
		return;
	}
	m_Yrotation = acos(glm::clamp(moveDirDotProduct, -1.f, 1.f)) * GTHTimes::Time_deltaTime * ROT_SPEED;
//	printf("ANGLE OF ROT>>>>>>>>> [%f]\n", m_Yrotation);
	if (wr_speed < ANIM_SEPARATOR)
	{////////////////////////////// IDLE ---> WALK
		speed = glm::clamp(wr_speed / ANIM_SEPARATOR, 0.f, 1.f);
		//printf("Velocity:::: %f\n", speed);
		m_playSpeed = glm::clamp(pow(1 + speed, 3 * (speed + 1))/ 10.f, 0.f, 1.f);
			
		//Dir == Fwd
		if (moveDirDotProduct> 0.95)
		{
			m_Yrotation = 0;
			m_idleBlendFact = 1 - speed;
			m_walkFwdBlendFact = speed;
			SavePrevAndBlendWithIt();//SavePrev();
			return;
		}
		//Dir == -Fwd
		if (moveDirDotProduct< -0.95)
		{
			m_idleBlendFact = 1 - speed;
			m_walkLeftBlendFact = speed;
			SavePrevAndBlendWithIt();//SavePrev();
			return;
		}

		if (moveDirDotProduct< 0)
		{
			moveDirDotProduct = 0;
		}

		if (moveDirCrossProduct.y < 0.0f)
		{//RIGHT
			m_Yrotation = -m_Yrotation;
			m_walkLeftBlendFact = speed * (1 - moveDirDotProduct);
			m_idleBlendFact = (1 - speed) * (1 - moveDirDotProduct);
			m_walkFwdBlendFact = 1 - m_walkLeftBlendFact - m_idleBlendFact;
		}
		else
		{//LEFT
			m_walkRightBlendFact = speed * (1 - moveDirDotProduct);
			m_idleBlendFact = (1 - speed) * (1 - moveDirDotProduct);
			m_walkFwdBlendFact= 1 - m_walkRightBlendFact - m_idleBlendFact;
		}
	}
	else //////////////////////-----------------------------------------------// WALK ---> RUN
	{
		speed = glm::clamp((wr_speed / 1.414213f - 0.56737f) / (1.f - 0.56737f) / 0.32f,0.f,1.f);
		//speed = glm::clamp(wr_speed, 0.f, 1.f);
		//printf("SPEEDMODE:::: %f\n", speed);
		m_playSpeed = glm::clamp(1.0f + speed/2, 0.f, 2.f);
		//printf("PLAY SPEED:::: %f\n", m_playSpeed);
		//Dir == Fwd
		if (moveDirDotProduct > 0.95)
		{
			m_Yrotation = 0;
			m_runFwdBlendFact = speed;
			m_walkFwdBlendFact = 1 - speed;
			SavePrevAndBlendWithIt();//SavePrev();
			return;
		}
		//Dir == -Fwd
		if (moveDirDotProduct < -0.95)
		{
			m_runLeftBlendFact = speed;
			m_walkLeftBlendFact = 1 - speed;
			SavePrevAndBlendWithIt();//SavePrev();
			return;
		}

		if (moveDirDotProduct< 0)
		{
			moveDirDotProduct = 0;
		}

		if (moveDirCrossProduct.y < 0.f)
		{//RIGHT
			m_Yrotation *= -1;
		
			//printf("RIGHT\n");

			m_runRightBlendFact = speed * (1 - moveDirDotProduct);
			m_walkRightBlendFact = (1 - speed) * (1 - moveDirDotProduct);

			m_runFwdBlendFact = speed * moveDirDotProduct;
			m_walkFwdBlendFact = (1 - speed) * moveDirDotProduct;
		}
		else //LEFT
		{
			//printf("LEFT\n");
			m_runLeftBlendFact = speed * (1 - moveDirDotProduct);
			m_walkLeftBlendFact = (1 - speed) * (1 - moveDirDotProduct);

			m_runFwdBlendFact = speed * moveDirDotProduct;
			m_walkFwdBlendFact = (1 - speed) * moveDirDotProduct;
		}
	}

	SavePrevAndBlendWithIt();
}

/////////////////////////////////////////////////////////////////////////////

float WalkRunAnimationState::GetRotation(float deltaTrans)
{
	if (fabs(m_Yrotation * 10.f) < 0.001f) return 0;

//	printf("DeltaTrans %f\n", deltaTrans);
	m_Yrotation /= 1 + deltaTrans * 10 / 3;

//		printf("ROTATION LOCALLY:: %f\n", m_Yrotation);
	return m_Yrotation;
}


void WalkRunAnimationState::SavePrevAndBlendWithIt()
{
	//return;
	prev_m_idleBlendFact = m_idleBlendFact = (float)((m_idleBlendFact * .16f) + (prev_m_idleBlendFact * 0.84f));
	prev_m_runFwdBlendFact = m_runFwdBlendFact = (float)((m_runFwdBlendFact * .16f) + (prev_m_runFwdBlendFact * 0.84f));
	prev_m_runLeftBlendFact = m_runLeftBlendFact = (float)((m_runLeftBlendFact * .16f) + (prev_m_runLeftBlendFact * 0.84f));
	prev_m_runRightBlendFact = m_runRightBlendFact = (float)((m_runRightBlendFact * .16f) + (prev_m_runRightBlendFact * 0.84f));
	prev_m_walkFwdBlendFact = m_walkFwdBlendFact = (float)((m_walkFwdBlendFact * .16f) + (prev_m_walkFwdBlendFact * 0.84f));
	prev_m_walkLeftBlendFact = m_walkLeftBlendFact = (float)((m_walkLeftBlendFact * .16f) + (prev_m_walkLeftBlendFact * 0.84f));
	prev_m_walkRightBlendFact = m_walkRightBlendFact = (float)((m_walkRightBlendFact * .16f) + (prev_m_walkRightBlendFact * 0.84f));
}

/////////////////////////////////////////////////////////////////////////////

void WalkRunAnimationState::SavePrev()
{
	prev_m_idleBlendFact = m_idleBlendFact;
	prev_m_runFwdBlendFact = m_runFwdBlendFact;
	prev_m_runLeftBlendFact = m_runLeftBlendFact;
	prev_m_runRightBlendFact = m_runRightBlendFact;
	prev_m_walkFwdBlendFact = m_walkFwdBlendFact;
	prev_m_walkLeftBlendFact = m_walkLeftBlendFact;
	prev_m_walkRightBlendFact = m_walkRightBlendFact;
}

/////////////////////////////////////////////////////////////////////////////

std::vector<std::pair<float, CharacterAnimation*> > WalkRunAnimationState::getBlendScheme()
{
	std::vector<std::pair<float, CharacterAnimation*> > scheme;
	bool empty = true;
	//	scheme.reserve(4);//TODO
#define BLEND_EPS 0.00001f
	if (fabs(m_idleBlendFact) > BLEND_EPS)
	{
		empty = false;
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_idleBlendFact, (CharacterAnimation*)m_idleAnimations[0]));
	}
	if (fabs(m_walkFwdBlendFact) > BLEND_EPS)
	{
		empty = false;
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_walkFwdBlendFact, (CharacterAnimation*)m_walkFwdAnim));
	}
	if (fabs(m_walkLeftBlendFact) > BLEND_EPS)
	{
		empty = false;
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_walkLeftBlendFact, (CharacterAnimation*)m_walkLeftAnim));
	}
	if (fabs(m_walkRightBlendFact) > BLEND_EPS)
	{
		empty = false;
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_walkRightBlendFact, (CharacterAnimation*)m_walkRightAnim));
	}
	if (fabs(m_runFwdBlendFact) > BLEND_EPS)
	{
		empty = false;
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_runFwdBlendFact, (CharacterAnimation*)m_runFwdAnim));
	}
	if (fabs(m_runLeftBlendFact) > BLEND_EPS)
	{
		empty = false;
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_runLeftBlendFact, (CharacterAnimation*)m_runLeftAnim));
	}
	if (fabs(m_runRightBlendFact) > BLEND_EPS)
	{
		empty = false;
		scheme.push_back(std::pair<float, CharacterAnimation*>(m_runRightBlendFact, (CharacterAnimation*)m_runRightAnim));
	}

	if (empty)
	{
		scheme.push_back(std::pair<float, CharacterAnimation*>(1.f, (CharacterAnimation*)m_idleAnimations[0]));
	}
//#define _PRINTDIRDEBUG
#ifdef _PRINTDIRDEBUG
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	printf("\t\tBLEND SMOOTHLY:: %f\n", smoothFact);
	printf("fwd(%f %f %f), dir(%f %f %f)\n)", m_characterFwdVecPtr->x, m_characterFwdVecPtr->y, m_characterFwdVecPtr->z, m_directionVecPtr->x, m_directionVecPtr->y, m_directionVecPtr->z);
	printf("IDLE blend fact: %f\n", m_idleBlendFact);
	printf("RUN  FORWARD blend fact: %f\n", m_runFwdBlendFact);
	printf("RUN  LEFT blend fact: %f\n", m_runLeftBlendFact);
	printf("RUN  RIGHT blend fact: %f\n\n\n", m_runRightBlendFact);
	printf("walk FORWARD blend fact: %f\n", m_walkFwdBlendFact);
	printf("walk LEFT blend fact: %f\n", m_walkLeftBlendFact);
	printf("walk RIGHT blend fact: %f\n\n\n", m_walkRightBlendFact);
	//	- m_walkFwdBlendFact - m_walkLeftBlendFact - m_walkRightBlendFact;
#endif
		
	return scheme;
}

float WalkRunAnimationState::GetDuration()
{
#define crtIDLE 0
	return this->m_runFwdAnim->m_duration_s;
	return this->m_idleAnimations[crtIDLE]->m_duration_s;
}
