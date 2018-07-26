#include "FloatingPlatform.h"
#include <LevelScene.h>

void IFloatingPlatform::Create() {
    Active = true;
    Priority = true;

    W = 48;
    H = 24;

    SolidTop = true;
    Scene->ObjectsSolid[Scene->ObjectSolidCount++] = this;


    Speed = 0;
    myX = X;
    myY = Y;
    if (FlipX) {
        // offset it by speeding up time
        for (int i = 0; i < 120; i++)
            Update();
    }

    Sprite = App->Sprites["Floating Platform"];

    if (Scene->ActualZone == 3) {
        W = 64;
        H = 56;
    }
}
void IFloatingPlatform::Update() {
    LastSpeed = Speed;
    if ((SubType & 0xF) == 1) {
        t++;
        t %= 240;
        mult = Math_PI * std::sin(t / 240.f * Math_PI * 2.f) / 240.f;

        Speed = mult * 64;
    }
    else if ((SubType & 0xF) == 2) {
        t++;
        t %= 240;
        mult = Math_PI * std::sin(t / 240.f * Math_PI * 2.f) / 240.f;

        Speed = mult * 128;
    }
    else if ((SubType & 0xF) == 3) {
        t++;
        t %= 240;
        mult = Math_PI * std::sin(t / 240.f * Math_PI * 2.f) / 240.f;

        Gravity = mult * -64;
    }
    else if ((SubType & 0xF) == 4) {
        t++;
        t %= 240;
        mult = Math_PI * std::sin(t / 240.f * Math_PI * 2.f) / 240.f;

        Gravity = mult * -128;
    }
    else if ((SubType & 0xF) == 0x7) {
        if (t == 1) {
            Gravity -= 1.f / 43.f;
        }
    }
    else if ((SubType & 0xF) == 0xC) {
        t++;
        t %= 240;
        mult = Math_PI * std::sin(t / 240.f * Math_PI * 2.f) / 240.f;

        Speed = mult * 192;
    }

    myX += Speed;
    myY += Gravity;
    X = myX;
    Y = myY;
}
void IFloatingPlatform::Render(int CamX, int CamY) {
    G->PaletteShift(0);
    G->DrawSprite(Sprite, (int)(X - LastSpeed - (Scene->MyPlayer->X - (int)Scene->MyPlayer->X)) - CamX, (int)(Y - Gravity) - CamY, 0.1f, (Scene->act & 0xF) - 1, IE::CenterAlign | IE::MiddleAlign);

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 1, 1, 0.5f));
    }
}

int  IFloatingPlatform::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    if (HitFrom == 1 && Scene->MyPlayer->Ground) {
        if ((SubType & 0xF) == 0x7) {
            if (t == 0) {
                t = 1;
            }
        }
    }
	return 1;
}
