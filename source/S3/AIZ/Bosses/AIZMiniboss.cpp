#include "AIZMiniboss.h"
#include <LevelScene.h>

void IAIZMiniboss::Create() {
    Active = true;
    Priority = true;

    W = 48;
    H = 48;

    Sprite = App->Sprites["Miniboss"];
    SpriteFlame = App->Sprites["Miniboss Flame"];
    SpriteSmall = App->Sprites["Miniboss Small"];
    Palette = App->Textures["Palette (Miniboss)"];

    HitCount = 6;

    Speed = 0;
    Gravity = 0;

    X += App->renderWidth - 320;
}
void IAIZMiniboss::Update() {
    if (HitCount > 0) {
        if (State == 0) {
            if (Scene->cameraX[0] >= 0x2F10 && Scene->MyPlayer->Ground) {
                if (FirstRun == -1) {
                    App->Stack->Push("Mini Boss", Scene->aud_BGM_Boss1, 10, 0, 3.0f, true, false);
                    FirstRun = 0;
                }
                State = 1;
                TimerAction = 0;
                Timer = 180;
            }
        }
        // Idling
        else if (State == 1) {

        }
        else if (State == 2) {
            Gravity = 0.25f * std::cos(Frame * Math_PI / 32.f);
            if ((int)X <= Scene->cameraX[App->CurrentViewport] + 0x28 && Speed != 0) {
                Speed = 0;

                TimerAction = 5;
                Timer = 20;
            }
            if ((int)X >= Scene->cameraX[App->CurrentViewport] + App->renderWidth - 0x28 && Speed != 0) {
                Speed = 0;

                TimerAction = 5;
                Timer = 20;
            }
            Flip = signum(X - (Scene->cameraX[App->CurrentViewport] + App->renderWidth / 2) + 0.01f);
        }
        // Waiting
        else if (State == 4) {
            Gravity = 0.25f * std::cos(Frame * Math_PI / 32.f);
        }
        // Moving downwards
        else if (State == 6) {
            Speed = 0;
            Gravity = 1;
        }
        // Moving upwards
        else if (State == 7) {
            Speed = 0;
            Gravity = -1;
        }
        // Fire Blast
        else if (State == 8) {
            Gravity = 0.25f * std::cos(Frame * Math_PI / 32.f);
        }

        if (Timer > -1)
            Timer--;

        if (Timer == 0) {
            int TA = TimerAction;
            TimerAction = 0;

            if (TA == 0) {
                State = 6;
                TimerAction = 1;
                Timer = 175;
            }
            else if (TA == 1) {
                // set to waiting
                State = 4;
                TimerAction = 2;
                Timer = 20 + 30; // apparently it does something within 20 frames, idk what
            }
            else if (TA == 2) {
                // set to fireblast
                State = 8;
                TimerAction = 3;
                Timer = 80;
            }
            else if (TA == 3) {
                // set to go up
                State = 7;
                TimerAction = 4;
                Timer = 80;
            }
            else if (TA == 4) {
                // set to moving sideways
                Speed = signum((Scene->cameraX[App->CurrentViewport] + App->renderWidth / 2) - X);
                State = 2;
            }
            else if (TA == 5) {
                // set to go down
                State = 6;
                TimerAction = 1;
                Timer = 80;
            }
        }

        X += Speed;
        Y += Gravity;
        Frame = (Frame + 1) % 64;

        if (HitTimer > -1)
            HitTimer--;
    }
    else {
        Scene->doSwitch = true;
    }


    // to start the load: Scene->beginActZoneTransfer((Scene->act & 0xF) + 1);
    // to do the switch:  Scene->doSwitch = true;
}
void IAIZMiniboss::Render(int CamX, int CamY) {
    G->PaletteShift(0);
    G->Scale(Flip, 1.f);
    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY + 32, 0.09f, 1 + (Scene->frameAnim % 4) / 2, IE::CenterAlign | IE::MiddleAlign);

    if (HitTimer > -1)
        G->PaletteShift(HitTimer % 2);
    G->SetNextSpriteDrawPalette(Palette, Palette);
    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.1f, 0, IE::CenterAlign | IE::MiddleAlign);

    // "Nose"
    G->DrawSprite(Sprite, (int)X - CamX - 36 * Flip, (int)Y - CamY + 8, 0.11f, 6, IE::CenterAlign | IE::MiddleAlign);

    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY - 32, 0.11f, 3, IE::CenterAlign | IE::MiddleAlign);
    G->DrawSprite(Sprite, (int)X - CamX + 9 * Flip, (int)Y - CamY - 28, 0.11f, 3, IE::CenterAlign | IE::MiddleAlign);
    G->DrawSprite(Sprite, (int)X - CamX + 18 * Flip, (int)Y - CamY - 24, 0.11f, 3, IE::CenterAlign | IE::MiddleAlign);

    G->SetNextSpriteDrawPalette(NULL, NULL);
    Scene->RestorePalette();
    G->Scale(1.f, 1.f);

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -0.1f, W, H, IColor(0.5f, 0.5f, 0.5f, 0.5f));
    }
}

int  IAIZMiniboss::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    if (!Scene->MyPlayer->Ground) {
        if (Scene->MyPlayer->Action == ActionType::Jumping ||
            Scene->MyPlayer->Action == ActionType::Glide) {
            int dis2 = (X - Scene->MyPlayer->X) * (X - Scene->MyPlayer->X);
            dis2    += (Y - Scene->MyPlayer->Y) * (Y - Scene->MyPlayer->Y);
            if (dis2 < 40 * 40 && HitTimer == -1) {
                HitTimer = 32;
                Scene->MyPlayer->Speed   = -Scene->MyPlayer->Speed;
                Scene->MyPlayer->Gravity = -Scene->MyPlayer->Gravity;
                if (Scene->MyPlayer->Action == ActionType::Glide) {
                    Scene->MyPlayer->Action = ActionType::GlideFall;
                }
                Scene->aud_BossHit->Play(1);
                if (HitCount == 1) {
                    Scene->beginActZoneTransfer((Scene->act & 0xF) + 1);

                    Scene->givePoint(1000, -1, -1, -1);
                }
                HitCount--;

                Scene->MyPlayer->Vibrate(0, 0.5, 500);
            }
        }
        else {
            Scene->MyPlayer->Hurt(X, false);
        }
    }
    else {
        Scene->MyPlayer->Hurt(X, false);
    }
	return 1;
}
