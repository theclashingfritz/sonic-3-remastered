#include "Standard.h"
#include "Input.h"
#include "Application.h"

#include "Engine/ITexture.h"
#include "Engine/IGraphics.h"
#include "Engine/IE.h"

// Private Methods
int Input::GetJoyFromName(const char* name) {
    for (int j = 0; j < MAX_JOYSTICKS; j++) {
        if (joysticks[j] != NULL) {
            if (strcmp(SDL_JoystickName(joysticks[j]), name) == 0) {
                return j;
            }
        }
    }
    return 0;
}

// Public Methods
void Input::Init() {
    for (int n = 0; n < MAX_JOYSTICKS; n++) {
        joysticks[n] = SDL_JoystickOpen(n);
        if (joysticks[n] != NULL) {
            app->print(0, "Joystick[%d] (%s) %s haptic support.", n, SDL_JoystickName(joysticks[n]), SDL_JoystickIsHaptic(joysticks[n]) == 1 ? "does have" : "does not have");
            haptics[n] = SDL_HapticOpenFromJoystick(joysticks[n]);
            if (haptics[n] != NULL) {
                SDL_HapticRumbleInit(haptics[n]);
            }
        }
    }

    for (int i = 0; i < 10; i++) {
        touchID[i] = -1;
    }


    const int JOYSTICK_DEAD_ZONE = 8000;
    if (!app->Mobile) {
        joyControls[0][0] = Control { "Controller (Xbox One For Windows)", 1, -1, -JOYSTICK_DEAD_ZONE * 3, -1, -1 };
    	joyControls[0][1] = Control { "Controller (Xbox One For Windows)", 1, -1, JOYSTICK_DEAD_ZONE * 2, 1, -1 };
    	joyControls[0][2] = Control { "Controller (Xbox One For Windows)", 0, -1, -JOYSTICK_DEAD_ZONE, -1, -1 };
    	joyControls[0][3] = Control { "Controller (Xbox One For Windows)", 0, -1, JOYSTICK_DEAD_ZONE, 1, -1 };
    	joyControls[0][4] = Control { "Controller (Xbox One For Windows)", -1, 0, SDL_PRESSED, 1, -1 };
    	joyControls[0][5] = Control { "Controller (Xbox One For Windows)", -1, 2, SDL_PRESSED, 1, -1 };
    	joyControls[0][6] = Control { "Controller (Xbox One For Windows)", -1, 1, SDL_PRESSED, 1, -1 };
    	joyControls[0][7] = Control { "Controller (Xbox One For Windows)", -1, 7, SDL_PRESSED, 1, -1 };
    }


    for (int n = 0; n < MAX_PLAYERS; n++) {
        keyControls[n][0] = Control { NULL, -1, SDL_SCANCODE_W, -1, -1, 0 };
        keyControls[n][1] = Control { NULL, -1, SDL_SCANCODE_S, -1, -1, 0 };
        keyControls[n][2] = Control { NULL, -1, SDL_SCANCODE_A, -1, -1, 0 };
        keyControls[n][3] = Control { NULL, -1, SDL_SCANCODE_D, -1, -1, 0 };
        keyControls[n][4] = Control { NULL, -1, SDL_SCANCODE_J, -1, -1, 0 };
        keyControls[n][5] = Control { NULL, -1, SDL_SCANCODE_K, -1, -1, 0 };
        keyControls[n][6] = Control { NULL, -1, SDL_SCANCODE_L, -1, -1, 0 };
        keyControls[n][7] = Control { NULL, -1, SDL_SCANCODE_I, -1, -1, 0 };

#if defined(PLATFORM_MAC)
        /*joyControls[n][0] = Control { "Joy-Con (L)", -1, -1, SDL_HAT_LEFT, 1, 0 };
		joyControls[n][1] = Control { "Joy-Con (L)", -1, -1, SDL_HAT_RIGHT, 1, 0 };
		joyControls[n][2] = Control { "Joy-Con (L)", -1, -1, SDL_HAT_DOWN, 1, 0 };
		joyControls[n][3] = Control { "Joy-Con (L)", -1, -1, SDL_HAT_UP, 1, 0 };*/
		joyControls[n][0] = Control { "Joy-Con (L)", -1,  2, SDL_PRESSED, 1, -1 };
		joyControls[n][1] = Control { "Joy-Con (L)", -1,  1, SDL_PRESSED, 1, -1 };
		joyControls[n][2] = Control { "Joy-Con (L)", -1,  0, SDL_PRESSED, 1, -1 };
		joyControls[n][3] = Control { "Joy-Con (L)", -1,  3, SDL_PRESSED, 1, -1 };

		joyControls[n][4] = Control { "Joy-Con (R)", -1,  0, SDL_PRESSED, 1, -1 };
		joyControls[n][5] = Control { "Joy-Con (R)", -1,  2, SDL_PRESSED, 1, -1 };
		joyControls[n][6] = Control { "Joy-Con (R)", -1,  1, SDL_PRESSED, 1, -1 };
		joyControls[n][7] = Control { "Joy-Con (R)", -1,  9, SDL_PRESSED, 1, -1 };
#endif
    }
}
void Input::Poll() {
    #define checkJoystick(n, id) (joyControls[n][id].Axis != -1 && \
    	SDL_JoystickGetAxis(joysticks[ \
    		GetJoyFromName(joyControls[n][id].Controller)], \
    		joyControls[n][id].Axis) * \
    		joyControls[n][id].Comparator > \
    		joyControls[n][id].Value * \
    		joyControls[n][id].Comparator) || \
    	(joyControls[n][id].Button != -1 && \
    	SDL_JoystickGetButton(joysticks[ \
    		GetJoyFromName(joyControls[n][id].Controller)], \
    		joyControls[n][id].Button)) || \
    	(joyControls[n][id].Hat != -1 && \
    	SDL_JoystickGetHat(joysticks[ \
    		GetJoyFromName(joyControls[n][id].Controller)], \
    		joyControls[n][id].Hat) == joyControls[n][id].Value)


    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

    for (int n = 0; n < MAX_PLAYERS; n++) {
        bool tempUp, tempDown, tempLeft, tempRight, tempJump1, tempJump2, tempJump3, tempPause;

		tempUp = currentKeyStates[keyControls[n][0].Button];
        tempDown = currentKeyStates[keyControls[n][1].Button];
		tempLeft = currentKeyStates[keyControls[n][2].Button];
		tempRight = currentKeyStates[keyControls[n][3].Button];
		tempJump1 = currentKeyStates[keyControls[n][4].Button];
		tempJump2 = currentKeyStates[keyControls[n][5].Button];
		tempJump3 = currentKeyStates[keyControls[n][6].Button];
		tempPause = currentKeyStates[keyControls[n][7].Button];

        if (joyControls[n][0].Controller != NULL && ((!onScreenControls && app->Mobile) || (onScreenControls)) ) {
            tempUp = tempUp || checkJoystick(n, 0);
    		tempDown = tempDown || checkJoystick(n, 1);
    		tempLeft = tempLeft || checkJoystick(n, 2);
    		tempRight = tempRight || checkJoystick(n, 3);
    		tempJump1 = tempJump1 || checkJoystick(n, 4);
    		tempJump2 = tempJump2 || checkJoystick(n, 5);
    		tempJump3 = tempJump3 || checkJoystick(n, 6);
    		tempPause = tempPause || checkJoystick(n, 7);
        }

        if (onScreenControls && n == 0) {
            for (int i = 0; i < 4; i++) {
                if (touchDown[i]) {
                    int X2 = 48;
                    int Y2 = app->renderHeight - 48;

                    int X3 = touchX[i];
                    int Y3 = touchY[i];

                    float angle = toDegrees(wrapAngleRad(std::atan2(Y2 - Y3, X3 - X2)));
                    float len = (Y2 - Y3) * (Y2 - Y3) + (X3 - X2) * (X3 - X2);

                    if (len < 100 * 100 && !app->realPaused) {
                        if (angle >= 45 && angle < 135) {
                            tempUp = true;
                        }
                        if (angle < 45 || angle >= 315) {
                            tempRight = true;
                        }
                        if (angle >= 135 && angle < 225) {
                            tempLeft = true;
                        }
                        if (angle >= 225 && angle < 315) {
                            tempDown = true;
                        }
                    }

                    X2 = app->renderWidth - 48 + 8;
                    Y2 = app->renderHeight - 48 + 8;
                    len = (Y2 - Y3) * (Y2 - Y3) + (X3 - X2) * (X3 - X2);

                    if (len < 75 * 75 && !app->realPaused) {
                        tempJump1 = true;
                    }

                    X2 = app->renderWidth / 2;
                    Y2 = 48;
                    len = (Y2 - Y3) * (Y2 - Y3) + (X3 - X2) * (X3 - X2);

                    if (len < 75 * 75) {
                        tempPause = true;
                    }
                }
            }
        }

        upDown[n] = false;
		downDown[n] = false;
		leftDown[n] = false;
		rightDown[n] = false;
		jumpDown[n] = false;
		jump2Down[n] = false;
		jump3Down[n] = false;
		pauseDown[n] = false;

		if (tempUp) {
			if (!up[n]) upPressed[n] = true;
			up[n] = true;
			upDown[n] = true;
		}
		if (tempDown) {
			if (!down[n]) downPressed[n] = true;
			down[n] = true;
			downDown[n] = true;
		}
		if (tempLeft) {
			if (!left[n]) leftPressed[n] = true;
			left[n] = true;
			leftDown[n] = true;
		}
		if (tempRight) {
			if (!right[n]) rightPressed[n] = true;
			right[n] = true;
			rightDown[n] = true;
		}

		if (tempJump1) {
			if (!jump[n]) jumpPressed[n] = true;
			jump[n] = true;
			jumpDown[n] = true;
		}

		if (tempJump2) {
			if (!jump2[n]) jump2Pressed[n] = true;
			jump2[n] = true;
			jump2Down[n] = true;
		}

		if (tempJump3) {
			if (!jump3[n]) jump3Pressed[n] = true;
			jump3[n] = true;
			jump3Down[n] = true;
		}

		if (tempPause) {
			if (!pause[n]) { pausePressed[n] = true; }
			pause[n] = true;
			pauseDown[n] = true;
		}

		if (!upDown[n])
			up[n] = false;
		if (!downDown[n])
			down[n] = false;
		if (!leftDown[n])
			left[n] = false;
		if (!rightDown[n])
			right[n] = false;
		if (!pauseDown[n])
			pause[n] = false;
		if (!jumpDown[n])
			jump[n] = false;
		if (!jump2Down[n])
			jump2[n] = false;
		if (!jump3Down[n])
			jump3[n] = false;
    }

    if (mouse) {
        if (!mouseDown) mousePressed = true;
        mouseDown = true;
    }

    if (!mouse)
        mouseDown = false;
}
void Input::Reset() {
    for (int n = 0; n < MAX_PLAYERS; n++) {
        upPressed[n] = false;
		downPressed[n] = false;
		leftPressed[n] = false;
		rightPressed[n] = false;
		jumpPressed[n] = false;
		jump2Pressed[n] = false;
		jump3Pressed[n] = false;
		pausePressed[n] = false;
    }

    for (int n = 0; n < 10; n++) {
        touchPressed[n] = false;
        touchReleased[n] = false;
    }

    mousePressed = false;
    mouseReleased = false;
    mouseMoved = false;
}
void Input::Free() {
    for (int n = 0; n < MAX_JOYSTICKS; n++) {
		if (haptics[n] != NULL) {
			SDL_HapticClose(haptics[n]);
			haptics[n] = NULL;
		}
		if (joysticks[n] != NULL) {
			SDL_JoystickClose(joysticks[n]);
			joysticks[n] = NULL;
		}
	}
}
void Input::Vibrate(int controller, double strength, int milliseconds) {
    if (app->Settings_Rumble)
        if (haptics[controller] != NULL)
            SDL_HapticRumblePlay(haptics[controller], strength, milliseconds);
}
void Input::DrawControls(ISprite* Sprite) {
    if (!onScreenControls)
        return;

    if (app->realPaused)
        return;

    app->g->Scale(1.f, 1.f);

    int DPADframe = 0;
    if (up[0])
        DPADframe = 1;
    else if (down[0])
        DPADframe = 2;
    else if (right[0])
        DPADframe = 3;
    else if (left[0]) {
        DPADframe = 3;
        app->g->Scale(-1.f, 1.f);
    }

    //app->g->SetBlend(1.f, 1.f, 1.f, 1.f);

    app->g->DrawSprite(Sprite, 48, app->renderHeight - 48, -1 * 5, DPADframe, IE::CenterAlign | IE::MiddleAlign);
    app->g->Scale(1.f, 1.f);

    app->g->DrawSprite(Sprite, app->renderWidth - 48, app->renderHeight - 48, -1 * 5, 4, IE::CenterAlign | IE::MiddleAlign);

    app->g->DrawSprite(Sprite, app->renderWidth - 76, 18, -1 * 5, 5, IE::CenterAlign | IE::MiddleAlign);

    //app->g->SetBlend(1.f, 1.f, 1.f, 1.f);
}
