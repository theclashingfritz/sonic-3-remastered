#include "RideVine.h"
#include <LevelScene.h>

void ISwingVine::Create() {
    Active = true;
    Priority = false;

    W = 16;
    H = 8;

    Sprite = App->Sprites["Swing Vine"];

    StartX = X;
    StartY = Y;
    BaseX = X;
    BaseY = Y;
    PotentialEnergy = 0;
    Timer = 0;

    Y += 64;
}

void ISwingVine::Update() {
    if (BaseX >= StartX + ((SubType & 0x7F) << 4) && PotentialEnergy == 0) {
        BaseX = StartX + ((SubType & 0x7F) << 4);
        if (Scene->ApparentAct == 1) {
            PotentialEnergy = 0;
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

    BaseX += Speed;
    BaseY += Gravity;

    Timer++;
    Timer %= 180;

    //Angle = std::sin(Timer * Math_PI / 128.f) * 4;
    Angle = std::sin(Timer / 180.f * 2 * Math_PI) * 90.f * ((SubType & 0xF0) == 0x60 ? -1 : 1);
    X = (int)(StartX + std::sin(Angle * Math_PI / 180.f) * 144);
    Y = (int)(StartY + std::cos(Angle * Math_PI / 180.f) * 144);
    /*if (Angle == 4)
        Angle = 3.99f;

    if (Speed > 0) {
        float scl = std::min(1.f, (X - StartX) / 512.f);
        Angle = std::sin(Timer * Math_PI / 128.f) * 2 - 10 * scl;
    } else if (PotentialEnergy > 0) {
        Angle = (Timer / 80.f) * 256.f;
    }*/
    
    if (Scene->MyPlayer->LastGrab == this && Scene->MyPlayer->ObjectControlled != 0) {
        Scene->MyPlayer->X = X;
        Scene->MyPlayer->Y = Y + 18;
        Scene->MyPlayer->Speed = signum(std::sin(Angle * Math_PI / 180.f)) * 2.5f;;

        //int A = (int)std::round(Angle);
        //int fr = A / 8;
        //float An = (int)(fr * 8) * 180 / 128.f;


        //Scene->MyPlayer->Angle = (int)An;
        Scene->MyPlayer->X = X + std::sin(0 * Math_PI / 180.f) * 20 + std::cos(0 * Math_PI / 180.f) * 2 * Scene->MyPlayer->Flip;
        Scene->MyPlayer->Y = Y + std::cos(0 * Math_PI / 180.f) * 20 + std::sin(0 * Math_PI / 180.f) * 2 * Scene->MyPlayer->Flip;

        //Angle = (int)(fr * 8);
    }
}

void ISwingVine::Render(int CamX, int CamY) {
    G->PaletteShift(0);
    // Handle
    G->DrawSprite(Sprite, (int)(X - CamX - (Scene->MyPlayer->X - (int)Scene->MyPlayer->X)), (int)(Y - CamY - (Scene->MyPlayer->Y - (int)Scene->MyPlayer->Y)), 0.0f, 0, 32, IE::CenterAlign | IE::MiddleAlign);

    // Rope?
    int OX = 0;
    int OY = 0;
    for (int i = 0; i < 9; i++) {
        int A = 0;
        if (PotentialEnergy == 0) {
            A = -(int)std::round(Angle);
        } else {
            A = -(int)std::round(Angle) * i;
        }
        int fr = A / 8;
        if (fr < 0)
            fr += 32;
        G->DrawSprite(Sprite, (int)(StartX - CamX + OX), (int)(StartY - CamY + OY), 0.1f - 5, 0, fr, IE::CenterAlign | IE::MiddleAlign);
        OX -= std::sin((int)(fr * 8) * Math_PI / 128.f) * 16;
        OY += std::cos((int)(fr * 8) * Math_PI / 128.f) * 16;
    }

    // Rope peg
    G->DrawSprite(Sprite, (int)StartX - CamX, (int)StartY - CamY, 0.1f, 33, IE::CenterAlign | IE::MiddleAlign);

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 1, 1, 0.5f));
    }
}

int  ISwingVine::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
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
    if (!Active)
        return 0;

    // 0618 secondary water level AIZ2

    Scene->MyPlayer->Speed = 0;
    Scene->MyPlayer->Gravity = 0;
    Scene->MyPlayer->X = X;
    Scene->MyPlayer->Y = Y + 18;
    Scene->MyPlayer->Action = ActionType::Grab;
    Scene->MyPlayer->HangingBar = true;
    Scene->MyPlayer->ObjectControlled = 3;
    Scene->MyPlayer->Flip = 1;
    Scene->MyPlayer->LastGrab = this;

    Scene->aud[0x4A]->Play(1);

    //if (PotentialEnergy == 0) {
    //    Speed = 8;
    //    Gravity = 2;
    //}

	return 1;
}