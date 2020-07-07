#include <System/System.h>
#include <System/EngineSystem.h>
#include <ctime>
#include <direct.h>
#include "SoundManager.h"
//#define KEYBOARD_BATCONTROL
//#define SHOW_ENEMY
#define GetCurrentDir _getcwd
//#define SHOW_COLLISION_BOXES
//#define  SLOW_LOADING


SystemEngine::SystemEngine() : 
mResourceManager(ResourceManager::getInstance()),
mSoundMgr(SoundManager::getInstance()),
mRenderSystem(RenderSystem::getInstance()),
mAnimationSystem(AnimationSystem::getInstance()),
mPhysicsSystem(PhysicsEngine::getInstance()),
mInputManager(InputManager::getInstance()),
mGameLogicSystem(GameLogicSystem::getInstance()), 
mObjectFactory(ObjectFactory::getInstance())
{}

void SystemEngine::Init(SCreationSettings&& cs)
{
	mResourceManager.Init();
	mPhysicsSystem.Init();

	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)//SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		exit(-1);
	}
	
	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	auto Width = DM.w;
	auto Height = DM.h;

	//printf("Window -- Width: %d, Height %d", Width, Height);
	m_window = SDL_CreateWindow(cs.windowTitle, 
								(Width - cs.windowWidth) / 2, (Height - cs.windowHeight) / 2, 
								cs.windowWidth, cs.windowHeight, 
								SDL_WINDOW_RESIZABLE | 
								SDL_WINDOW_OPENGL);// | SDL_WINDOW_BORDERLESS);
	m_context = SDL_GL_CreateContext(m_window);
	SDL_GL_SetSwapInterval(0);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_MakeCurrent(m_window, m_context);
	SDL_GL_SwapWindow(m_window);

	
	//srand(time(NULL));
	GTHTimes::TIME_Init();
	
	mSoundMgr.Init();
	mRenderSystem.Init(); 
	
	mRenderSystem.SplashScreen();
	SDL_GL_MakeCurrent(m_window, m_context);
	SDL_GL_SwapWindow(m_window);
	
	mInputManager.Init(m_window);
	mObjectFactory.Init();
	mRenderSystem.SetActiveCamera((Camera*)(&((Character*)(mGameLogicSystem.mScripts[0]))->camera));
	
	//SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
}


void ImportStaticCollisionShapes()
{
	Assimp::Importer importer;
	
	
}

void SystemEngine::PhysicsSetup()
{
	ImportStaticCollisionShapes();
	
	return;
}

void SystemEngine::Start()
{
	const int TICKS_PER_SECOND = 60;
	const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
	const int MAX_FRAMESKIP = 5;

	Uint64 next_game_tick = GTHTimes::GetTickCount();
	int loops;

	while (true) 
	{
		SDL_PumpEvents();
		loops = 0;
		
		while (GTHTimes::GetTickCount() > next_game_tick && loops < MAX_FRAMESKIP)
		{//Step Game Simulation	
			SystemFixedUpdate();
			SystemLateUpdate();
			next_game_tick += SKIP_TICKS;
			loops++;
		}
		//Render World
		mRenderSystem.RenderScene();
		SDL_GL_MakeCurrent(m_window, m_context);
		SDL_GL_SwapWindow(m_window);
	}
}
void SystemEngine::Cleanup()
{

}

void SystemEngine::SystemFixedUpdate()
{
	GTHTimes::TIME_Tick();
	mObjectFactory.FixedUpdate();
	mInputManager.FixedUpdate(m_window);
	mAnimationSystem.FixedUpdate();
	mGameLogicSystem.FixedUpdate();

	PhysicsEngine &physicsEngine = PhysicsEngine::getInstance();
	if (GTHTimes::Time_deltaTime < 1.f / 60.f * 5.f)
	{
		physicsEngine.dynamicsWorld->stepSimulation(GTHTimes::Time_deltaTime, 5);
	}
	else
	{
		physicsEngine.dynamicsWorld->stepSimulation(GTHTimes::Time_deltaTime, int(GTHTimes::Time_deltaTime * 60.f) + 1);
	}

	//if (GTHTimes::Time_deltaTime >= 1.f / 6.f) printf("WARNING: BELOW 6 FPS\n");
	
}

void SystemEngine::SystemLateUpdate()
{
	mInputManager.LateUpdate();
}