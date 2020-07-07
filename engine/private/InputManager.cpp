#include <InputManager.h>
#include <math.h>
#include <cstdio>
#include <RenderingSystem.h>
#include <GameLogicSystem.h>
#include <SoundManager.h>
#include <ObjectFactory.h>
#include <Component/Camera.h>
glm::vec2 SquareSpace2CircleSpace(const glm::vec2 &in)
{
	//printf("IN____________ (%5.2f, %5.2f)\n", in.x, in.y);
	glm::vec2 v = glm::normalize(in);
	glm::vec2 absv = glm::vec2(fabs(in.x), fabs(in.y));

	if (absv.x < 0.008f)
		return v * absv.y;
	else if (absv.y < 0.008f)
		return v * absv.x;

	float tgt = absv.y / absv.x;
	if (fabs(tgt - 1.f) < 0.008f)
		return v * glm::length(in) / sqrt(2.f);
	if (tgt > 1.f)
		return v * glm::length(in) / sqrt(1.f + 1.f / (tgt * tgt));
	else
		return v * glm::length(in) / sqrt(1.f + (tgt * tgt));
}

#define GAMEPAD_ENABLED
#define NATECGENESIS_PV58
//#define THRUSTMASTER_DualAnalog3

#if defined   NATECGENESIS_PV58
const int JOYSTICK_DEAD_ZONE = INT16_MAX;
#elif defined THRUSTMASTER_DualAnalog3
const int JOYSTICK_DEAD_ZONE = (int16_t)((float)INT16_MAX * 2.f / 3.f);
#endif
void InputManager::FixedUpdate(SDL_Window *pWindow)
{
	if (mEditorMode) {
		float moveSpeed = 20.f;
		if (LCtrlPressed)
			moveSpeed = 100.f;
		if (keyState['w'])
			mRenderSystem.mActiveCamera->translateForward(moveSpeed * GTHTimes::Time_deltaTime);
		else if (keyState['s'])
			mRenderSystem.mActiveCamera->translateForward(-moveSpeed * GTHTimes::Time_deltaTime);
		
		if (keyState['a'])
			mRenderSystem.mActiveCamera->translateRight(-moveSpeed * GTHTimes::Time_deltaTime);
		else if (keyState['d'])
			mRenderSystem.mActiveCamera->translateRight(moveSpeed * GTHTimes::Time_deltaTime);
	}
	int mouseDX, mouseDY,width, height;
	int dir;
	int w, h;
	SDL_Event evt;
	SDL_Window *win = NULL;

	while (SDL_PollEvent(&evt))
	{
		win = SDL_GetWindowFromID(evt.window.windowID);
		if (mEditorMode)
		{
			ImGui_ImplSdlGL3_ProcessEvent(&evt);
			
		}
		
		
		switch (evt.type)
		{
		case SDL_KEYDOWN:
		{
			NotifyKeyPressed(evt);
			break;
		}
		case SDL_KEYUP:
		{
			NotifyKeyReleased(evt);
			break;
		}
		case SDL_WINDOWEVENT:
			switch (evt.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
			{	if (win != pWindow)
					break;
				SDL_GetWindowSize(win, &w, &h);
				mRenderSystem.SetWindowResolution(w, h);
				break;
			}
			case SDL_WINDOWEVENT_CLOSE:
			{
				SDL_Window *window = SDL_GetWindowFromID(evt.window.windowID);
				if (window)
				{
					SDL_DestroyWindow(window);
				}
				break;
			}
			break;
			}
			break;
		case SDL_JOYAXISMOTION:
		{
			NotifyAxisMotion(evt);
			break;
		}
		case SDL_JOYBUTTONDOWN:
		{
			NotifyButtonDown(evt);
			break;
		}
		case SDL_JOYBUTTONUP:
		{
			NotifyButtonUp(evt);
			break;
		}
		case SDL_JOYDEVICEADDED:
		{
			NotifyDeviceConnected(evt);
			break;
		}
		case SDL_JOYDEVICEREMOVED:
		{
			NotifyDeviceRemoved(evt);
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
			//if (win == window) printf("BTn Down LMB\n");
			switch (evt.button.button)
			{
			case SDL_BUTTON_LEFT:
				LMB_Down = LMB_Pressed = true;
				break;
			case SDL_BUTTON_RIGHT:
				RMB_Down = RMB_Pressed = true;
				break;
			case SDL_BUTTON_MIDDLE:
				MMB_Down = MMB_Pressed = true;
				break;
			default:
				//SDL_ShowSimpleMessageBox(0, "Mouse", "Some other button was pressed!", window);
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			//if (win == window) printf("BTn Down RMB\n");
			switch (evt.button.button)
			{
			case SDL_BUTTON_LEFT:
				LMB_Pressed = false;
				LMB_Up = true;
				break;
			case SDL_BUTTON_RIGHT:
				RMB_Pressed = false;
				RMB_Up = true;
				break;
			case SDL_BUTTON_MIDDLE:
				MMB_Pressed = false;
				MMB_Up = true;
				break;
			default:
				//SDL_ShowSimpleMessageBox(0, "Mouse", "Some other button was pressed!", window);
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			SDL_GetWindowSize(win, &width, &height);

			
			dir = -1;
			if (mEditorMode)
			{
				dir = 1;
				mouseDX = dir * evt.motion.xrel;
				mouseDY = dir * -evt.motion.yrel;
			}
			else
			{
				mouseDX = evt.motion.x - width / 2;
				mouseDY = height / 2 - evt.motion.y;
			}
			//printf("%d vs %d ...... %d vs %d\n", mouseDX, evt.motion.x - width / 2, mouseDY, height / 2 - evt.motion.y);
			if (mouseDX == 0 && mouseDY == 0)
				return;

			if (mEditorMode)
			{
				mRenderSystem.mActiveCamera->rotateFPS_OX((float)mouseDY);
				mRenderSystem.mActiveCamera->rotateFPS_OY((float)mouseDX);
			}
			if (keyboardControl) {
				m_CameraMovement = glm::vec2((float)mouseDX, (float)mouseDY);
			}
			if (!mEditorMode && win == pWindow)
				SDL_WarpMouseInWindow(pWindow, width / 2, height / 2);
			break;
		
		default:
			break;

		}
		
	}


}

void InputManager::LateUpdate()
{
	PrepareForNextFrame();
}

InputManager::InputManager() : mRenderSystem(RenderSystem::getInstance()),
mGameLogicSystem(GameLogicSystem::getInstance()),
mSoundManager(SoundManager::getInstance())
{
}
InputManager::~InputManager ()
{
	ImGui_ImplSdlGL3_Shutdown();
	SDL_JoystickClose(0);
}

void InputManager::Init(SDL_Window *pWindow)
{
	mWindow = pWindow;
	memset(freshState, 0, 256 * sizeof(char));
	memset(keyState, 0, 256 * sizeof(bool));
	
	
	ImGui_ImplSdlGL3_Init(pWindow);

	// Setup style
	ImGui::StyleColorsClassic();
	//ImGui::StyleColorsDark();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'extra_fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);
	ImGui_ImplSdlGL3_NewFrame(pWindow);
	int width, height;
	SDL_GetWindowSize(pWindow, &width, &height);

	
	gGameController = NULL;
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_JOYSTICK) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	//Set texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		printf("Warning: Linear texture filtering not enabled!");
	}

	//Check for joysticks
	if (SDL_NumJoysticks() < 1)
	{
		printf("Warning: No joysticks connected!\n");
		keyboardControl = true;
	}
	else
	{
		keyboardControl = false;
		printf("Loading Gamepad...\n");
		//Load joystick
		gGameController = SDL_JoystickOpen(0);
		if (gGameController == NULL)
		{
			printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
		}
	}
	SDL_ShowCursor(0);
}

void InputManager::NotifyButtonDown(const SDL_Event &evt)
{
	switch (evt.jbutton.button)
	{
	case 0:
		m_XIsPressed = true;
		m_XDown = true;
		break;
	case 1:
		m_SqIsPressed = true;
		m_SqDown = true;
		break;
	case 2:
		m_OIsPressed = true;
		m_ODown = true;
		break;
	case 3:
		m_TriIsPressed = true;
		m_TriDown = true;
		break;
	case 4:
		m_L1IsPressed = true;
		m_L1Down = true;
		break;
	case 5:
		m_L2IsPressed = true;
		m_L2Down = true;
		break;
	case 6:
		m_R1IsPressed = true;
		m_R1Down = true;
		break;
	case 7:
		m_R2IsPressed = true;
		m_R2Down = true;
		break;
	case 8:
		m_L3IsPressed = true;
		m_L3Down = true;
		break;
	case 9:
		m_R3IsPressed = true;
		m_R3Down = true;
		break;
	case 10:
		m_L4IsPressed = true;
		m_L4Down = true;
		break;
	case 11:
		m_R4IsPressed = true;
		m_R4Down = true;
		break;
	}
}

void InputManager::NotifyButtonUp(const SDL_Event &evt)
{
	switch (evt.jbutton.button)
	{
	case 0:
		m_XIsPressed = false;
		m_XUp = true;
		break;
	case 1:
		m_SqIsPressed = false;
		m_SqUp = true;
		break;
	case 2:
		m_OIsPressed = false;
		m_OUp = true;
		break;
	case 3:
		m_TriIsPressed = false;
		m_TriUp = true;
		break;
	case 4:
		m_L1IsPressed = false;
		m_L1Up = true;
		break;
	case 5:
		m_L2IsPressed = false;
		m_L2Up = true;
		break;
	case 6:
		m_R1IsPressed = false;
		m_R1Up = true;
		break;
	case 7:
		m_R2IsPressed = false;
		m_R2Up = true;
		break;
	case 8:
		m_L3IsPressed = false;
		m_L3Up = true;
		break;
	case 9:
		m_R3IsPressed = false;
		m_R3Up = true;
		break;
	case 10:
		m_L4IsPressed = false;
		m_L4Up = true;
		break;
	case 11:
		m_R4IsPressed = false;
		m_R4Up = true;
		break;
	}
}


void InputManager::NotifyDeviceConnected(const SDL_Event &evt)
{
	//TODO
	Init(SDL_GetWindowFromID(evt.window.windowID));
	printf("Input Manager>>> Gamepad connected.\n");
	
}

void InputManager::NotifyDeviceRemoved(const SDL_Event &evt)
{
	keyboardControl = true;
	SDL_JoystickClose(0);
	printf("Input Manager>>> Gamepad removed!\n");
}

inline float clampJoystickAxis(Sint16 value){
	if (value <= -JOYSTICK_DEAD_ZONE)
	{
		return -1.f;
	}
	else if (value >= JOYSTICK_DEAD_ZONE)
	{
		return 1.f;
	}
//	printf("INT16 VALUE: %d\n", value);
	return (float)value / (float)INT16_MAX;
}

void InputManager::NotifyAxisMotion(const SDL_Event &evt)
{
#ifdef GAMEPAD_ENABLED
	if (true || evt.jaxis.which == 0) //Motion on L_StickController
	{
		if (evt.jaxis.axis == 0) //walk/run velocity on Zaxis
		{

			m_XoZMovement[0] = clampJoystickAxis(evt.jaxis.value);
		}
		if (evt.jaxis.axis == 1)
		{
			m_XoZMovement[1] = clampJoystickAxis(evt.jaxis.value);
		}

		if (evt.jaxis.axis == 2) //walk/run velocity on Zaxis
		{

#if defined THRUSTMASTER_DualAnalog3
			m_CameraMovement[0] = clampJoystickAxis(evt.jaxis.value);
#elif defined NATECGENESIS_PV58	
#endif
		}
		if (evt.jaxis.axis == 3)
		{
#if defined THRUSTMASTER_DualAnalog3
			m_CameraMovement[1] = -clampJoystickAxis(evt.jaxis.value);
#elif defined NATECGENESIS_PV58
			m_CameraMovement[0] = clampJoystickAxis(evt.jaxis.value);
#endif
		}
		if (evt.jaxis.axis == 4)
		{
#if defined NATECGENESIS_PV58
			m_CameraMovement[1] = -clampJoystickAxis(evt.jaxis.value);	
#endif
		}
		if (evt.jaxis.axis == 5)
		{
			//TODO 
		}
		else if (evt.jaxis.axis == 6)
		{
		}
		if (evt.jaxis.axis == 7)
		{
		}
	}
#endif
}
void InputManager::NotifyKeyPressed(SDL_Event &evt)
{	
	SDL_Keycode key = evt.key.keysym.sym;

	if (key >= -128 && key <= 127)
	{
		freshState[key] = 1;
		keyState[key] = true;
	}
	switch (key) {

	case SDLK_F1: {
		m_displayMode = (m_displayMode + 1) % 2;
		Uint32 fsflag;
		switch (m_displayMode) {
		case 0:
			SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
			break;
		case 1:
			SDL_SetWindowFullscreen(mWindow, 0);
			break;
		default:
			break;
		}
		break;
	}
	case SDLK_F2:
	{
		GTHTimes::TIME_pause = !GTHTimes::TIME_pause;
		break;
	}
	case SDLK_F4:
	{

		mEditorMode = !mEditorMode;
		if (mEditorMode)
		{
			mRenderSystem.SetActiveCamera(mRenderSystem.mDefaultCamera);
			SDL_ShowCursor(1);
	
		}
		else
		{
			
			mRenderSystem.SetActiveCamera((Camera*)(&((Character*)(mGameLogicSystem.mScripts[0]))->camera));
			SDL_ShowCursor(0);
		}
		break;
		

	}
	case SDLK_F3:
	{
		mSoundManager.muted = !mSoundManager.muted;
		break;
	}
	case SDLK_r:
	{
		ObjectFactory &objFactory = ObjectFactory::getInstance();

		objFactory.ResetGame();
		break;
	}
	case SDLK_LCTRL:
	{
		LCtrlDown = LCtrlPressed = true;
		break;
	}
	case SDLK_F5: {
		//	glm::vec3 p = RenderSystem.shader_bank.currentShader->point_lights[0]->position;
		mRenderSystem.reload();
		SDL_Window *window = SDL_GetWindowFromID(evt.window.windowID);
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		mRenderSystem.SetWindowResolution(w, h);
		break;
	}
	case SDLK_p: {
		//	glm::vec3 p = RenderSystem.shader_bank.currentShader->point_lights[0]->position;
		GTHTimes::TIME_pause = !GTHTimes::TIME_pause;
		break;
	}
	case SDLK_j: {
		//	glm::vec3 p = RenderSystem.shader_bank.currentShader->point_lights[0]->position;
		GTHTimes::GLOBAL_orthoFar += 1.f;
		printf("ORTHO FAR: %f\n", GTHTimes::GLOBAL_orthoFar);
		break;
	}
	case SDLK_n: {
		//	glm::vec3 p = RenderSystem.shader_bank.currentShader->point_lights[0]->position;
		GTHTimes::GLOBAL_orthoFar -= 1.f;
		printf("ORTHO FAR: %f\n", GTHTimes::GLOBAL_orthoFar);
		break;
	}
	case SDLK_l: {
		//	glm::vec3 p = RenderSystem.shader_bank.currentShader->point_lights[0]->position;
		GTHTimes::GLOBAL_orthoNear += 1.f;
		printf("ORTHO NEAR : %f\n", GTHTimes::GLOBAL_orthoNear);
		break;
	}
	case SDLK_m: {
		//	glm::vec3 p = RenderSystem.shader_bank.currentShader->point_lights[0]->position;
		GTHTimes::GLOBAL_orthoNear -= 1.f;
		printf("ORTHO NEAR : %f\n", GTHTimes::GLOBAL_orthoNear);
		break;
	}
	case SDLK_ESCAPE :
	{
		SDL_DestroyWindow(SDL_GetWindowFromID(evt.window.windowID));
		exit(0);
		//	glut::close();
	}

	}
}

void InputManager::NotifyKeyReleased(SDL_Event &evt)
{
	SDL_Keycode key = evt.key.keysym.sym;
	if (key == SDLK_LCTRL)
	{
		LCtrlPressed = false;
		LCtrlUp = true;
	}
	if (key >= -128 && key <= 127) {
		keyState[key] = false;
		freshState[key] = 2;
	}
}

void InputManager::PrepareForNextFrame()
{
	if(keyboardControl)
	{
		m_XoZMovement = glm::vec2(0, 0);
		m_CameraMovement = glm::vec2(0, 0);
		if (keyState['w'])
			m_XoZMovement += glm::vec2(0, -1);
		if (keyState['s'])
			m_XoZMovement += glm::vec2(0, 1);
		if (keyState['a'])
			m_XoZMovement += glm::vec2(-1, 0);
		if (keyState['d'])
			m_XoZMovement += glm::vec2(1, 0);

		if (LCtrlPressed)
			m_XoZMovement *= 0.6;

	}

	memset(freshState, 0, 256 * sizeof(char));
	
	LCtrlDown = LCtrlUp = false;
	LMB_Down = LMB_Up = false;
	RMB_Down = RMB_Up = false;
	MMB_Down = MMB_Up = false;
	m_XDown = m_ODown = m_SqDown = m_TriDown = m_L1Down = m_L2Down =
	m_L3Down = m_L4Down = m_R1Down = m_R2Down = m_R3Down = m_R4Down =
	m_LeftDown = m_RightDown = m_UpwardDown = m_DownwardDown =
	m_XUp = m_OUp = m_SqUp = m_TriUp = m_L1Up = m_L2Up =
	m_L3Up = m_L4Up = m_R1Up = m_R2Up = m_R3Up = m_R4Up =
	m_LeftUp = m_RightUp = m_UpwardUp = m_UpwardUp = false;
}