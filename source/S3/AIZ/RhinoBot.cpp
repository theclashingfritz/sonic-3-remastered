#include "RhinoBot.h"
#include <LevelScene.h>

void IRhinoBot::Create() {
    Active = true;
    Priority = false;

    W = 32;
    H = 32;

    Sprite = App->Sprites["RhinoBot"];

    Scene->ObjectsEnemies[Scene->ObjectEnemiesCount++] = this;

    Frame = 0;
    Speed = 0;
    Acceleration = -0.0625f;
}
void IRhinoBot::Update() {
    if (Speed == signum(Acceleration) * 3) {
        Acceleration = -Acceleration;
        Scene->aud[0x48]->Play(2);
    }

    if (Speed == 0 && Timer == -1 && std::abs(Speed) < 4) {
        FlipX = !FlipX;
        if (FlipX)
            Acceleration = -0.0625f;
        else
            Acceleration = 0.0625f;
    }

    Speed += Acceleration;
    X += Speed;

    if (Timer == -1 && std::abs(Speed) < 4) {
        if (signum(Speed) == signum(Acceleration)) {
            Frame = 0;
            if (std::abs(Speed) < 0.5f) {
                Frame = 3;
            }
            if (std::abs(Speed) < 0.25f) {
                Frame = 1;
            }
        }
        else {
            Frame = 1;
            if (std::abs(Speed) < 2.5f) {
                Frame = 2;
            }
        }

        for (int p = 0; p < Scene->PlayerCount; p++) {
            IPlayer* Player = Scene->GetPlayer(p);
            int DisX = (Player->X - X) * signum(Speed);
            int DisY = std::abs(Player->Y - Y);
            if (DisX < 96 && DisX > 0 && DisY <= 32) {
                Timer = 24;
                Speed = 0;
                Acceleration = 0;
                Frame = 0;
            }
        }
    }

    if (Timer == 0) {
        if (FlipX)
            Speed = -4.f;
        else
            Speed = 4.f;
        Frame = 0;
    }

    int ang;
    int Y1 = (int)Y;
    int Y2 = Y1;
    for (int y = (int)Y + 8; y < (int)Y + 32; y++) {
        if (Scene->CollisionAt((int)(X - 8), y, 0, &ang, false, NULL, false)) {
            Y1 = y;
            break;
        }
    }
    for (int y = (int)Y + 8; y < (int)Y + 32; y++) {
        if (Scene->CollisionAt((int)(X + 8), y, 0, &ang, false, NULL, false)) {
            Y2 = y;
            break;
        }
    }

    if (Y1 < Y2 && Y1 > Y + 8) {
        Y = Y1 - 16;
    }
    else if (Y2 > Y + 8) {
        Y = Y2 - 16;
    }
    if (Y1 >= Y + 30 || Y1 == Y || Scene->CollisionAt((int)(X - 8 - 1), Y, 0, &ang, false, NULL, false)) {
        Acceleration = -0.0625f * signum(Speed);
        Speed = 0;
        Speed += Acceleration;
        Timer = -1;
        X += 1;
        FlipX = false;
    }
    if (Y2 >= Y + 30 || Y2 == Y || Scene->CollisionAt((int)(X + 8 + 1), Y, 0, &ang, false, NULL, false)) {
        Acceleration = -0.0625f * signum(Speed);
        Speed = 0;
        Speed += Acceleration;
        Timer = -1;
        X -= 1;
        FlipX = true;
    }


    if (Timer >= 0)
        Timer--;
}
void IRhinoBot::Render(int CamX, int CamY) {
    float Flip = FlipX ? 1.f : -1.f;

    G->PaletteShift(0);
    if (Frame == 3)
        Flip = -Flip;
    G->Scale(Flip, 1.0f);
    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.1f, Frame, IE::CenterAlign | IE::MiddleAlign);
    if (Frame == 2)
        G->DrawSprite(Sprite, (int)X - CamX + Flip * 8, (int)Y - CamY + 8, 0.09f, 4 + (Scene->frameAnim / 3) % 4, IE::CenterAlign | IE::MiddleAlign);
    if (Timer != -1)
        G->DrawSprite(Sprite, (int)X - CamX + Flip * 16, (int)Y - CamY + 8, 0.09f, 4 + (Scene->frameAnim / 3) % 4, IE::CenterAlign | IE::MiddleAlign);
    G->Scale(1.0f, 1.0f);

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 0, 0, 0.5f));
    }
}
