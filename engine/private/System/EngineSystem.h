#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <Config.h>

#include <GameObjects/Character.h>
#include <RenderingSystem.h>
#include <GlobalTime.h>
#include <InputManager.h>
#include <HUD.h>
#include <PhysicsSystem.h>
#include <SoundManager.h>
#include <GameLogicSystem.h>
#include <ObjectFactory.h>
#include <ResourceManager.h>
#include <AnimationSystem.h>
#define PLATFORM_COLOR_1 0.625f/randColFact,0.625f/randColFact,1.0f/randColFact
#define PLATFORM_COLOR_2 1.0f/randColFact,1.0f/randColFact,0.625f/randColFact

struct SCreationSettings;

class ENGINE_API SystemEngine
{
	SoundManager &mSoundMgr;
	RenderSystem &mRenderSystem;
	AnimationSystem &mAnimationSystem;
	PhysicsEngine &mPhysicsSystem;
	InputManager &mInputManager;
	GameLogicSystem &mGameLogicSystem;
	ObjectFactory &mObjectFactory;
	ResourceManager &mResourceManager;
public:
	SystemEngine();
	void Init(SCreationSettings&& cs);
	void Start();
	void Cleanup();
	void PhysicsSetup();
	void SystemFixedUpdate();
	void SystemLateUpdate();
	
protected:
	SDL_Window* m_window;
	SDL_GLContext m_context;
};