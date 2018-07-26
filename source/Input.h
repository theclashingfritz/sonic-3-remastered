#ifndef INPUT_H
#define INPUT_H

#include "Standard.h"

#include "Engine/ITexture.h"
#include "Engine/IGraphics.h"
#include "Engine/IE.h"

class Input {
private:
    struct Control { const char* Controller; int Axis; int Button; int Value; int Comparator; int Hat; };
    static const int MAX_JOYSTICKS = 8;

    SDL_Joystick* joysticks[MAX_JOYSTICKS];
    SDL_Haptic* haptics[MAX_JOYSTICKS];

    Control keyControls[4][8];
    Control joyControls[4][8];

    bool upDown[4];
    bool downDown[4];
    bool leftDown[4];
    bool rightDown[4];
    bool jumpDown[4];
    bool jump2Down[4];
    bool jump3Down[4];
    bool pauseDown[4];

public:
    Application* app;

    int mouseX = -1;
    int mouseY = -1;
    bool mouse = false;
    bool mouseDown = false;
    bool mousePressed = false;
    bool mouseReleased = false;
    bool mouseMoved = false;
    bool mousePrimary = true;

    bool up[4];
    bool upPressed[4];
    bool down[4];
    bool downPressed[4];
    bool left[4];
    bool leftPressed[4];
    bool right[4];
    bool rightPressed[4];
    bool jump[4];
    bool jumpPressed[4];
    bool jump2[4];
    bool jump2Pressed[4];
    bool jump3[4];
    bool jump3Pressed[4];
    bool pause[4];
    bool pausePressed[4];

    int touchX[10];
    int touchY[10];
	bool touchDown[10];
	bool touchPressed[10];
	bool touchReleased[10];
	long long touchID[10];

    bool onScreenControls = true;

	int MAX_PLAYERS = 1;

    int GetJoyFromName(const char* name);
    void Init();
    void Poll();
    void Reset();
    void Free();
    void Vibrate(int controller, double strength, int milliseconds);

    void DrawControls(ISprite* sprite);
};

#endif
