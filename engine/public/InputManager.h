#pragma once
#include <SDL.h>
#include "../../../dependencies/glm/glm.hpp"
#include <unordered_map>
#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>

#define MIN_JOYSTICK_SENSITIVITY 0.02

class RenderSystem;
class GameLogicSystem;
class SoundManager;

glm::vec2 SquareSpace2CircleSpace(const glm::vec2 &in);

#define DEFINE_GAMEPAD_PUSH_BUTTON(butName, key)							\
private:																\
	bool m_##butName##Down = false, m_##butName##Up = false, m_##butName##IsPressed = false;\
	char m_##butName##Key = ##key##; \
public:																	\
inline bool Get##butName##Down() const {if(keyboardControl)return freshState[m_##butName##Key] == 1;else return this->m_##butName##Down;}	\
inline bool Get##butName##Up() const {if(keyboardControl)return freshState[m_##butName##Key] == 2;else return this->m_##butName##Up;}	\
inline bool Get##butName##IsPressed() const {if(keyboardControl)return keyState[m_##butName##Key];else return this->m_##butName##IsPressed;}\

class InputManager
{
public:
	bool mEditorMode = false;
	bool keyState[256];
	char freshState[256];
	bool keyboardControl;
	bool LCtrlPressed = false, LCtrlDown = false, LCtrlUp = false;
	bool LMB_Pressed = false, LMB_Down = false, LMB_Up = false;
	bool RMB_Pressed = false, RMB_Down = false, RMB_Up = false;
	bool MMB_Pressed = false, MMB_Down = false, MMB_Up = false;
	DEFINE_GAMEPAD_PUSH_BUTTON(X, 'i')
	DEFINE_GAMEPAD_PUSH_BUTTON(O, 'o')
	DEFINE_GAMEPAD_PUSH_BUTTON(Sq, 'p')
	DEFINE_GAMEPAD_PUSH_BUTTON(Tri, 'j')
	DEFINE_GAMEPAD_PUSH_BUTTON(L1, 'k')
	DEFINE_GAMEPAD_PUSH_BUTTON(L2, 'l')
	DEFINE_GAMEPAD_PUSH_BUTTON(L3, 'b')
	DEFINE_GAMEPAD_PUSH_BUTTON(L4, 'n')
	DEFINE_GAMEPAD_PUSH_BUTTON(R1, 'm')
	DEFINE_GAMEPAD_PUSH_BUTTON(R2, ',')
	DEFINE_GAMEPAD_PUSH_BUTTON(R3, '.')
	DEFINE_GAMEPAD_PUSH_BUTTON(R4, ';')
	DEFINE_GAMEPAD_PUSH_BUTTON(Left, '\'')
	DEFINE_GAMEPAD_PUSH_BUTTON(Right, 'h')
	DEFINE_GAMEPAD_PUSH_BUTTON(Upward, 'g')
	DEFINE_GAMEPAD_PUSH_BUTTON(Downward, 'v')
private:
	glm::vec2 m_XoZMovement, m_CameraMovement;
public:
	inline glm::vec2 GetXoZMovement() const {
		if (fabs(m_XoZMovement.x) < 0.008 && fabs(m_XoZMovement.y) < 0.008) return glm::vec2(0, 0);
		return SquareSpace2CircleSpace(this->m_XoZMovement);
	}
	inline glm::vec2 GetCameraMovement() const {
		if (fabs(m_CameraMovement.x) < 0.008 && fabs(m_CameraMovement.y) < 0.008) return glm::vec2(0, 0);
		return this->m_CameraMovement * (keyboardControl?0.25f:1.f);
	}

private:
	InputManager();
	InputManager(InputManager const& copy);            // do NOT implement
	InputManager& operator=(InputManager const& copy); // do NOT implement
public:
	// Stop the compiler from generating copy methods
	static InputManager& getInstance()
	{
		static InputManager instance;
		return instance;
	}

	~InputManager();

	void Init(SDL_Window *pWindow);
	void NotifyAxisMotion(const SDL_Event &evt);
	void NotifyButtonDown(const SDL_Event &evt);
	void NotifyButtonUp(const SDL_Event &evt);
	void NotifyDeviceConnected(const SDL_Event &evt);
	void NotifyDeviceRemoved(const SDL_Event &evt);
	void PrepareForNextFrame();
	void NotifyKeyPressed(SDL_Event &evt);
	void NotifyKeyReleased(SDL_Event &evt);
	void FixedUpdate(SDL_Window *window);
	void LateUpdate();
private:
	RenderSystem &mRenderSystem;
	GameLogicSystem &mGameLogicSystem;
	SoundManager &mSoundManager;
	int m_displayMode = 0;
	
	SDL_Joystick *gGameController;
public:
	SDL_Window *mWindow;
};