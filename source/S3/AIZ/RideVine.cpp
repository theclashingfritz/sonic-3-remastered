#include "RideVine.h"
#include <LevelScene.h>

void IRideVine::Create() {
    Active = true;
    Priority = false;

    W = 16;
    H = 16;

    Sprite = App->Sprites["Swing Vine"];

    StartX = X;
    StartY = Y;
    BaseX = X;
    BaseY = Y;
    PotentialEnergy = 0;
    Timer = 0;

    Y += 64;
}
void IRideVine::Update() {
    if (BaseX >= StartX + ((SubType & 0x7F) << 4) && PotentialEnergy == 0) {
        BaseX  = StartX + ((SubType & 0x7F) << 4);
        if (Scene->ApparentAct == 1) {
            PotentialEnergy = 8;
        } else {
            Active = false;
            Scene->MyPlayer->ObjectControlled = 0;
            Scene->MyPlayer->Ground = false;
            Scene->MyPlayer->Action = ActionType::Jumping;
            return;
        }
        Speed = 0;
        Gravity = 0;
        Timer = 0;
        Priority = true;
    }

    X = BaseX - Speed;
    Y = BaseY - Gravity + 64;

    BaseX += Speed;
    BaseY += Gravity;

    if (PotentialEnergy == 0) {
        Timer = (Timer + 2) % 256;
    } else {
        Timer = (Timer + 1) % 80;
    }

    Angle = std::sin(Timer * Math_PI / 128.f) * 4;
    if (Angle == 4)
        Angle = 3.99f;

    if (Speed > 0) {
        float scl = std::min(1.f, (X - StartX) / 512.f);
        Angle = std::sin(Timer * Math_PI / 128.f) * 2 - 10 * scl;
    }
    else if (PotentialEnergy > 0) {
        Angle = (Timer / 80.f) * 256;
    }

    int OX = 0;
    int OY = 0;
    if (PotentialEnergy == 0) {
        for (int i = 0; i < 4; i++) {
            int A = -(int)std::round(Angle) * i;
            int fr = A / 8;
            OX -= std::sin((int)(fr * 8) * Math_PI / 128.f) * 16;
            OY += std::cos((int)(fr * 8) * Math_PI / 128.f) * 16;
        }
    } else {
        for (int i = 0; i < 4; i++) {
            int A = -(int)std::round(Angle);
            int fr = A / 8;
            OX -= std::sin((int)(fr * 8) * Math_PI / 128.f) * 16;
            OY += std::cos((int)(fr * 8) * Math_PI / 128.f) * 16;
        }
    }

    X = BaseX + OX;
    Y = BaseY + OY;
    if (Scene->MyPlayer->LastGrab == this && Scene->MyPlayer->ObjectControlled != 0) {
        Scene->MyPlayer->X = X;
        Scene->MyPlayer->Y = Y + 18;
        Scene->MyPlayer->Speed = 8;
        if (PotentialEnergy > 0) {
            int A = (int)std::round(Angle);
            int fr = A / 8;
            float An = (int)(fr * 8) * 180 / 128.f;


            Scene->MyPlayer->Angle = (int)An;
            Scene->MyPlayer->X = X + std::sin(An * Math_PI / 180.f) * 18;
            Scene->MyPlayer->Y = Y + std::cos(An * Math_PI / 180.f) * 18;

            Angle = (int)(fr * 8);
        }
    }
}
void IRideVine::Render(int CamX, int CamY) {
    G->PaletteShift(0);
    // Handle
    G->DrawSprite(Sprite, (int)(X - CamX), (int)(Y - CamY), -0.3f, PotentialEnergy > 0 ? std::round(Angle) * Math_PI / 128.f : 0, 32, IE::CenterAlign | IE::MiddleAlign);

    // Rope?
    int OX = 0;
    int OY = 0;
    for (int i = 0; i < 4; i++) {
        int A = 0;
        if (PotentialEnergy == 0) {
            A = -(int)std::round(Angle) * i;
        } else {
            A = -(int)std::round(Angle);
        }
        int fr = A / 8;
        if (fr < 0)
            fr += 32;
        G->DrawSprite(Sprite, (int)BaseX - CamX + OX, (int)BaseY - CamY + OY, 0.1f - 5, 0, fr, IE::CenterAlign | IE::MiddleAlign);
        OX -= std::sin((int)(fr * 8) * Math_PI / 128.f) * 16;
        OY += std::cos((int)(fr * 8) * Math_PI / 128.f) * 16;
    }

    // Rope peg
    G->DrawSprite(Sprite, (int)BaseX - CamX, (int)BaseY - CamY, 0.1f, 33, IE::CenterAlign | IE::MiddleAlign);

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 1, 1, 0.5f));
    }
}

int  IRideVine::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    if (Scene->MyPlayer->X - X + 16 >= 32)
        return 0;
    if (Scene->MyPlayer->Y - Y >= 24)
        return 0;
    if (Scene->MyPlayer->Ground)
        return 0;
    if (Scene->MyPlayer->ObjectControlled != 0)
        return 0;
    if (Scene->MyPlayer->LastGrab == this)
        return 0;
    if (Scene->DebugMode != 0)
        return 0;
    if (!Active)
        return 0;

    // 0618 secondary water level AIZ2

    Scene->MyPlayer->Speed = 0;
    Scene->MyPlayer->Gravity = 0;
    Scene->MyPlayer->X = X;
    Scene->MyPlayer->Y = Y + 18;
    Scene->MyPlayer->Action = ActionType::Grab;
    Scene->MyPlayer->ObjectControlled = 3;
    Scene->MyPlayer->Flip = 1;
    Scene->MyPlayer->LastGrab = this;

    Scene->aud[0x4A]->Play(1);

    if (PotentialEnergy == 0) {
        Speed = 8;
        Gravity = 2;
    }

	return 1;
}

void IZiplinePeg::Create() {
    Active = true;
    Priority = false;

    W = 64;
    H = 24;

    Sprite = App->Sprites["Zipline Peg"];
}

void IZiplinePeg::Render(int CamX, int CamY) {
    G->PaletteShift(0);
    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.1f, 0, IE::CenterAlign | IE::MiddleAlign);
}